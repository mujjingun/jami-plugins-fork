/**
 *  Copyright (C) 2020 Savoir-faire Linux Inc.
 *
 *  Author: Aline Gondim Santos <aline.gondimsantos@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA.
 */

#include "videoSubscriber.h"
// Use for display rotation matrix
extern "C" {
#include <libavutil/buffer.h>
#include <libavutil/display.h>
#include <libavutil/frame.h>
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
}

// opencv
#include <opencv2/imgproc.hpp>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

// LOGGING
#include <pluglog.h>

#include "model/utf.hpp"

static const std::string TAG = "AutoSub";

namespace jami {

static FT_Library freetype = [] {
    FT_Library library;
    FT_Init_FreeType(&library);
    return library;
}();

struct VideoSubscriberPimpl {
    FT_Face face;
};

VideoSubscriber::VideoSubscriber(const std::string& dataPath, MessageQueue* queue)
    : path_{dataPath}
    , queue(queue)
    , pimpl(new VideoSubscriberPimpl{})
{
    auto font_path = (dataPath + separator() + "KoPubDotumBold.ttf");
    auto err = FT_New_Face(freetype, font_path.c_str(), 0, &pimpl->face);
    if (err) {
        Plog::log(Plog::LogPriority::ERR, TAG, "Error loading font: " + font_path);
    } else {
        Plog::log(Plog::LogPriority::INFO, TAG, "Font loaded successfully: " + font_path);
    }
    err = FT_Set_Pixel_Sizes(pimpl->face, 0, 20);
    if (err) {
        Plog::log(Plog::LogPriority::ERR, TAG, "Error setting font size");
    } else {
        Plog::log(Plog::LogPriority::INFO, TAG, "Font size set successfully");
    }
}

VideoSubscriber::~VideoSubscriber()
{
    stop();

    // join processing thread
    Plog::log(Plog::LogPriority::INFO, TAG, "~MediaProcessor");
}

void rotate_image(cv::Mat& mat, int angle)
{
    if (angle == -90) {
        cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
    } else if (std::abs(angle) == 180) {
        cv::rotate(mat, mat, cv::ROTATE_180);
    } else if (angle == 90) {
        cv::rotate(mat, mat, cv::ROTATE_90_CLOCKWISE);
    }
}

void drawText(cv::Mat& mat, FT_Face face, int baseline, std::string const& u8str)
{
    // convert to utf32
    std::u32string str;
    utf::stringview(u8str.begin(), u8str.end()).to<utf::utf32>(std::back_inserter(str));

    // estimate text size
    // TODO: break lines
    int width = 0;
    int height = 0;
    int descender = 0;
    for (std::size_t n = 0; n < str.size(); ++n) {
        auto err = FT_Load_Char(face, str[n], FT_LOAD_RENDER);
        if (err) {
            Plog::log(Plog::LogPriority::ERR, TAG, "Error loading char: " + std::to_string(static_cast<int>(str[n])));
            continue;
        }

        err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (err) {
            Plog::log(Plog::LogPriority::ERR, TAG, "Error rendering glyph: " + std::to_string(static_cast<int>(str[n])));
        }

        width += face->glyph->advance.x >> 6;
        height = std::max(height, face->glyph->bitmap_top);
        descender = std::max(descender, static_cast<int>(face->glyph->bitmap.rows - face->glyph->bitmap_top));
    }

    // set text start position
    cv::Point pos(baseline, mat.rows / 2 - width / 2);

    // draw background
    cv::rectangle(mat,
        cv::Point(pos.x - descender - 5, pos.y - 10),
        cv::Point(pos.x + height + 5, pos.y + width + 10),
        cv::Scalar::all(0), cv::FILLED);

    // draw text
    for (std::size_t n = 0; n < str.size(); ++n) {
        auto err = FT_Load_Char(face, str[n], FT_LOAD_RENDER);
        if (err) {
            Plog::log(Plog::LogPriority::ERR, TAG, "Error loading char: " + std::to_string(static_cast<int>(str[n])));
            continue;
        }

        err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (err) {
            Plog::log(Plog::LogPriority::ERR, TAG, "Error rendering glyph: " + std::to_string(static_cast<int>(str[n])));
        }

        for (unsigned i = 0; i < face->glyph->bitmap.rows; ++i) {
            for (unsigned j = 0; j < face->glyph->bitmap.width; ++j) {
                int x = pos.y + face->glyph->bitmap_left + j;
                int y = pos.x + face->glyph->bitmap_top - i;
                auto color = face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];

                if (x >= 0 && y >= 0 && x < mat.rows && y < mat.cols) {
                    mat.at<cv::Vec3b>(x, y) = cv::Vec3b::all(color);
                }
            }
        }

        pos.y += face->glyph->advance.x >> 6;
    }
}

static FrameUniquePtr transferToMainMemory(AVFrame* framePtr, AVPixelFormat desiredFormat)
{
    FrameUniquePtr out{av_frame_alloc(), [](AVFrame* frame) {
                           if (frame) {
                               av_frame_free(&frame);
                           }
                       }};

    auto desc = av_pix_fmt_desc_get(static_cast<AVPixelFormat>(framePtr->format));

    if (desc && not(desc->flags & AV_PIX_FMT_FLAG_HWACCEL)) {
        return FrameUniquePtr{framePtr, [](AVFrame*) {}};
    }

    out->format = desiredFormat;
    if (av_hwframe_transfer_data(out.get(), framePtr, 0) < 0) {
        return FrameUniquePtr{framePtr, [](AVFrame*) {}};
    }

    out->pts = framePtr->pts;
    if (AVFrameSideData* side_data = av_frame_get_side_data(framePtr, AV_FRAME_DATA_DISPLAYMATRIX)) {
        av_frame_new_side_data_from_buf(out.get(),
            AV_FRAME_DATA_DISPLAYMATRIX,
            av_buffer_ref(side_data->buf));
    }
    return out;
}

void VideoSubscriber::update(jami::Observable<AVFrame*>*, AVFrame* const& iFrame)
{
    if (!iFrame) {
        return;
    }

    //======================================================================================
    // GET FRAME ROTATION
    AVFrameSideData* side_data = av_frame_get_side_data(iFrame, AV_FRAME_DATA_DISPLAYMATRIX);

    int angle{0};
    if (side_data) {
        auto matrix_rotation = reinterpret_cast<int32_t*>(side_data->data);
        angle = static_cast<int>(av_display_rotation_get(matrix_rotation));
    }

    auto mem_ptr = transferToMainMemory(iFrame, AV_PIX_FMT_NV12);
    FrameUniquePtr frame_ptr = scaler.convertFormat(mem_ptr.get(), AV_PIX_FMT_RGB24);

    auto frame = cv::Mat(
        frame_ptr->height,
        frame_ptr->width,
        CV_8UC3,
        frame_ptr->data[0],
        static_cast<std::size_t>(frame_ptr->linesize[0]));

    // obtain result image
    rotate_image(frame, angle + 90);

    auto msg = queue->message();
    if (msgs.back() != msg) {
        msgs.push_back(msg);
        msgs.pop_front();
    }
    drawText(frame, pimpl->face, frame.cols / 4, msgs[0]);
    drawText(frame, pimpl->face, frame.cols / 4 - 30, msgs[1]);

    rotate_image(frame, -angle - 90);

    // REPLACE AVFRAME DATA WITH FRAME DATA
    std::uint8_t* ptr = frame_ptr->data[0];
    for (int i = 0; i < frame_ptr->height; ++i) {
        for (int j = 0; j < frame_ptr->width; ++j) {
            auto v = frame.at<cv::Vec3b>(i, j);
            ptr[3 * j + 0] = v[0];
            ptr[3 * j + 1] = v[1];
            ptr[3 * j + 2] = v[2];
        }
        ptr += frame_ptr->linesize[0];
    }

    // Copy Frame meta data
    av_frame_copy_props(frame_ptr.get(), iFrame);
    scaler.moveFrom(iFrame, frame_ptr.get());
}

void VideoSubscriber::attached(jami::Observable<AVFrame*>* observable)
{
    std::ostringstream oss;
    oss << "::Attached ! " << std::endl;
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
    observable_ = observable;

    // reset state
    msgs.clear();
    msgs.push_back("");
    msgs.push_back("");
}

void VideoSubscriber::detached(jami::Observable<AVFrame*>*)
{
    observable_ = nullptr;
    std::ostringstream oss;
    oss << "::Detached()" << std::endl;
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
}

void VideoSubscriber::detach()
{
    if (observable_) {
        std::ostringstream oss;
        oss << "::Calling detach()" << std::endl;
        Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
    }
}

void VideoSubscriber::stop()
{
}

} // namespace jami

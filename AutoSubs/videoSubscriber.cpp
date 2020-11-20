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
#include <accel.h>
#include <libavutil/display.h>
}

// opencv
#include <opencv2/imgproc.hpp>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

// LOGGING
#include <pluglog.h>

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

VideoSubscriber::VideoSubscriber(const std::string& dataPath)
    : path_{dataPath}
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
    std::ostringstream oss;
    oss << "~MediaProcessor" << std::endl;
    stop();

    // join processing thread
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
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

void drawText(cv::Mat& mat, FT_Face face, std::u32string const& str)
{
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

    cv::Point pos(mat.cols / 4, mat.rows / 2 - width / 2);

    cv::rectangle(mat,
        cv::Point(pos.x - descender - 5, pos.y - 10),
        cv::Point(pos.x + height + 5, pos.y + width + 10),
        cv::Scalar::all(0), cv::FILLED);

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
                mat.at<cv::Vec3b>(x, y) = cv::Vec3b::all(color);
            }
        }

        pos.y += face->glyph->advance.x >> 6;
    }
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
        /*
        Plog::log(Plog::LogPriority::INFO, TAG,
            "matrix = \n[[" + std::to_string(matrix_rotation[0]) + " " + std::to_string(matrix_rotation[1]) + " " + std::to_string(matrix_rotation[2]) + "]\n"
                + "[" + std::to_string(matrix_rotation[3]) + " " + std::to_string(matrix_rotation[4]) + " " + std::to_string(matrix_rotation[5]) + "]\n"
                + "[" + std::to_string(matrix_rotation[6]) + " " + std::to_string(matrix_rotation[7]) + " " + std::to_string(matrix_rotation[8]) + "]]\n"
                + "angle = " + std::to_string(angle));
        */
    }

    FrameUniquePtr frame_ptr = scaler.convertFormat(
        transferToMainMemory(iFrame, AV_PIX_FMT_NV12), AV_PIX_FMT_RGB24);

    auto frame = cv::Mat(
        frame_ptr->height,
        frame_ptr->width,
        CV_8UC3,
        frame_ptr->data[0],
        static_cast<std::size_t>(frame_ptr->linesize[0]));

    // obtain result image
    rotate_image(frame, angle + 90);

    std::u32string str = U"Hello안녕";
    drawText(frame, pimpl->face, str);

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

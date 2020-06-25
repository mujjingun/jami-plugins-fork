/*
 *  Copyright (C) 2004-2020 Savoir-faire Linux Inc.
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 */

#pragma once
extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

//STL
#include <memory>
#include <functional>

using FrameUniquePtr = std::unique_ptr<AVFrame, void(*)(AVFrame*)>;

class FrameScaler{
public:
    FrameScaler() : ctx_(nullptr), mode_(SWS_FAST_BILINEAR) {}

    /**
     * @brief scaleConvert
     * Scales an av frame accoding to the desired width height/height
     * Converts the frame to another format if the desiredFromat is different from the input PixelFormat
     * @param input
     * @param desiredWidth
     * @param desiredHeight
     * @param desiredFormat
     * @return
     */
    FrameUniquePtr scaleConvert(const AVFrame* input, const size_t desiredWidth, const size_t desiredHeight,
                                const AVPixelFormat desiredFormat){
        FrameUniquePtr output{av_frame_alloc(), [](AVFrame* frame){ if(frame) {av_frame_free(&frame);} }};
        if(input) {
            output->width = static_cast<int>(desiredWidth);
            output->height = static_cast<int>(desiredHeight);
            output->format = static_cast<int>(desiredFormat);

            auto output_frame = output.get();

            if (av_frame_get_buffer(output_frame, 0))
                throw std::bad_alloc();

            ctx_ = sws_getCachedContext(ctx_,
                                        input->width,
                                        input->height,
                                        static_cast<AVPixelFormat>(input->format),
                                        output_frame->width,
                                        output_frame->height,
                                        static_cast<AVPixelFormat>(output_frame->format),
                                        mode_,
                                        nullptr, nullptr, nullptr);
            if (!ctx_) {
                throw std::bad_alloc();
            }

            sws_scale(ctx_, input->data, input->linesize, 0,
                      input->height, output_frame->data,
                      output_frame->linesize);
        }

        return output;
    }

    /**
     * @brief convertFormat
     * @param input
     * @param pix
     * @return
     */
    FrameUniquePtr convertFormat(const AVFrame* input, AVPixelFormat pix) {
        return input?scaleConvert(input,static_cast<size_t>(input->width),static_cast<size_t>(input->height), pix):
                     std::unique_ptr<AVFrame, void(*)(AVFrame*)>{nullptr, [](AVFrame* frame){(void)frame;}};
    }

    /**
     * @brief moveFrom
     * @param dst
     * @param src
     */
    void moveFrom(AVFrame* dst,  AVFrame* src) {
        if(dst && src) {
            av_frame_unref(dst);
            av_frame_move_ref(dst, src);
        }
    }

protected:
    SwsContext *ctx_;
    int mode_;
};

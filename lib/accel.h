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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 */

extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/frame.h>
#include <libavutil/buffer.h>
}

namespace DRing {
class MediaFrame;
class VideoFrame;
} // namespace DRing

namespace jami {
using MediaFrame = DRing::MediaFrame;
using VideoFrame = DRing::VideoFrame;
} // namespace jami

extern "C" {
#if LIBAVUTIL_VERSION_MAJOR < 56
AVFrameSideData*
av_frame_new_side_data_from_buf(AVFrame* frame, enum AVFrameSideDataType type, AVBufferRef* buf)
{
    auto side_data = av_frame_new_side_data(frame, type, 0);
    av_buffer_unref(&side_data->buf);
    side_data->buf = buf;
    side_data->data = side_data->buf->data;
    side_data->size = side_data->buf->size;
    return side_data;
}
#endif
}

AVFrame*
transferToMainMemory(AVFrame* framePtr, AVPixelFormat desiredFormat)
{
    AVFrame* out = av_frame_alloc();
    auto desc = av_pix_fmt_desc_get(static_cast<AVPixelFormat>(framePtr->format));

    if (desc && not(desc->flags & AV_PIX_FMT_FLAG_HWACCEL)) {
        out = framePtr;
        return out;
    }

    out->format = desiredFormat;
    if (av_hwframe_transfer_data(out, framePtr, 0) < 0) {
        out = framePtr;
        return out;
    }

    out->pts = framePtr->pts;
    if (AVFrameSideData* side_data = av_frame_get_side_data(framePtr, AV_FRAME_DATA_DISPLAYMATRIX)) {
        av_frame_new_side_data_from_buf(out,
                                        AV_FRAME_DATA_DISPLAYMATRIX,
                                        av_buffer_ref(side_data->buf));
    }
    return out;
}

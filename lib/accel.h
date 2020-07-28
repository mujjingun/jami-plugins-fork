

#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/frame.h>
#include <libavutil/buffer.h>
#include "pluglog.h"

namespace DRing {
class MediaFrame;
class VideoFrame;
}

namespace jami {
using MediaFrame = DRing::MediaFrame;
using VideoFrame = DRing::VideoFrame;
}

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
    AVFrame* out = nullptr;

    auto desc = av_pix_fmt_desc_get(static_cast<AVPixelFormat>(framePtr->format));
    if (desc && not (desc->flags & AV_PIX_FMT_FLAG_HWACCEL)) {
        out = framePtr;
        return out;
    }

    out->format = desiredFormat;
    
    if (av_hwframe_transfer_data(out, framePtr, 0) < 0) {
        out = framePtr;
        return out;
    }

    out->pts = framePtr->pts;
    if (AVFrameSideData* side_data = av_frame_get_side_data(framePtr, AV_FRAME_DATA_DISPLAYMATRIX))
        av_frame_new_side_data_from_buf(out, AV_FRAME_DATA_DISPLAYMATRIX, av_buffer_ref(side_data->buf));
    return out;
}
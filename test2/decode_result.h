#ifndef DECODE_RESULT_H
#define DECODE_RESULT_H

extern "C" {
#   include <libavcodec/avcodec.h>
}

class DecodeResult
{
public:
    virtual int save_video_frame(const AVFrame *frame) = 0;
};

#endif // DECODE_RESULT_H

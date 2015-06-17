#ifndef TEST_FFMPEG_H
#define TEST_FFMPEG_H

#include "ffmpegwrap.h"
#include <QThread>

class test_ffmpeg : public QThread
                  , public ffmpegDecoderCallback
{
    ffmpegWrap *media_;

public:
    test_ffmpeg();

private:
    virtual void run();
    virtual int on_audio_frame(int idx, const AVFrame *frame);
    virtual int on_video_frame(int idx, const AVFrame *frame);
};

#endif // TEST_FFMPEG_H

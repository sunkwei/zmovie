#include "test_ffmpeg.h"

test_ffmpeg::test_ffmpeg()
{
    start();
}

void test_ffmpeg::run()
{
    media_ = new ffmpegWrap("rtsp://172.16.1.52/av0_0", this);

    while (1) {
        media_->run_once();
    }
}

int test_ffmpeg::on_audio_frame(int idx, const AVFrame *frame)
{
    return 0;
}

int test_ffmpeg::on_video_frame(int idx, const AVFrame *frame)
{
    return 0;
}

#ifndef FFMPEGWRAP_H
#define FFMPEGWRAP_H

extern "C" {
#   include <libavcodec/avcodec.h>
#   include <libavformat/avformat.h>
#   include <libswscale/swscale.h>
#   include <libswresample/swresample.h>
#   include <libavutil/avutil.h>
#   include <libavutil/opt.h>
}
#include <string>

class ffmpegDecoderCallback
{
public:
    virtual int on_video_frame(int idx, const AVFrame *frame, double stamp) = 0;
    virtual int on_audio_frame(int idx, const AVFrame *frame, double stamp) = 0;
};

class ffmpegWrap
{
    std::string url_;
    ffmpegDecoderCallback *cb_;
    AVFormatContext *ic_;
    SwsContext *sws_;
    AVFrame *frame_;
    AVPacket pkg_;
    uint64_t audio_sample_cnt_;    // 为了计算时间戳 ...

public:
    enum {
        RC_OK = 0,
        RC_FAILURE = -1,
        RC_MEDIA_END = 1,   // 文件播放结束...
    };

    ffmpegWrap(const char *url, ffmpegDecoderCallback *cb);
    ~ffmpegWrap();

    /** 需要调用者不停地调用 .... */
    int run_once();

private:
    int open_url();
    int next_frame();
    int decode_frame(AVStream *stream, AVPacket *pkt);
};

#endif // FFMPEGWRAP_H

#ifndef MEDIASOURCE_H
#define MEDIASOURCE_H

#include <QThread>
#include <string>
extern "C" {
#   include <libavformat/avformat.h>
#   include <libavcodec/avcodec.h>
#   include <libswscale/swscale.h>
}

class MediaSource : public QThread
{
    Q_OBJECT

signals:
    void one_frame(bool is_audio, int sid, AVFrame *frame);

public:
    MediaSource(const char *fname);
    virtual ~MediaSource();

private:
    std::string fname_;
    AVFormatContext *ctx_;
    bool quit_;
    bool playing_;
    typedef int (*pfn_decode)(AVCodecContext *, AVFrame *, int*, const AVPacket *);
    pfn_decode decode_video_, decode_audio_;

public:
    int open();
    void close();

    int play();  // 连续播放
    int pause();  //

private:
    virtual void run(); //
    void decode_one_frame(AVPacket &pkg);
};

#endif // MEDIASOURCE_H

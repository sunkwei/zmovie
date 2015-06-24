#ifndef MEDIATHREAD_H
#define MEDIATHREAD_H

#include <QThread>
#include <string>
#include "ffmpegwrap.h"
#include <algorithm>
#include <deque>
#include <QMutex>
#include <QWaitCondition>
#include <QImage>
#include "utils.h"
#include <assert.h>
#include "kvconfig2.h"

/** 启动工作线程，保存解码帧 ...
 */
class MediaThread : public QThread
                  , ffmpegDecoderCallback
{
    Q_OBJECT

public:
    class Picture
    {
        double stamp_;
        int width_, height_;
        int input_width_, input_height_;
        AVPicture pic_;
        QImage *image_;
        SwsContext *sws_;

    public:
        Picture();
        ~Picture();

        void save(const AVFrame *frame, double stamp);
        QImage *image() const;
        double stamp() const;

        int width() const { return width_; }
        int height() const { return height_; }
        const AVPicture &pic() const { return pic_; }
    };

    class Pcm
    {
        double stamp_;
        int ch_, bitsize_, samplerate_;
        unsigned char *buf_;
        int buf_len_, data_len_;
        SwrContext *swr_;   // 总是转换为 S16 格式的声音 ....

    public:
        Pcm();
        ~Pcm();

        int sample_rate() const { return samplerate_; }

        void save(const AVFrame *frame, double stamp);
        void *data(int &len) const;
        double stamp() const;
    };

    MediaThread(const char *url);
    virtual ~MediaThread();

    size_t video_pending_size();
    double video_pending_duration();
    double video_pending_first_stamp();
    Picture *lock_picture();
    void unlock_picture(Picture *p);

    size_t audio_pending_size();
    double audio_pending_duration();
    double audio_pending_first_stamp();
    int audio_pending_next_bytes(); // 返回fifo中第一个的字节长度 ...
    Pcm *lock_pcm();
    void unlock_pcm(Pcm *pcm);

private:
    virtual void run();
    virtual int on_audio_frame(int idx, const AVFrame *frame, double stamp);
    virtual int on_video_frame(int idx, const AVFrame *frame, double stamp);

    Picture *next_freed_picture();
    void save_data_picture(Picture *p);
    Picture *next_picture();
    void save_freed_picture(Picture *p);
    void prepare_cache(int n);
    void release_all_buf();

    Pcm *next_freed_pcm();
    void save_data_pcm(Pcm *p);

private:
    bool quit_;
    std::string url_;

    typedef std::deque<Picture*> PICTURES;
    PICTURES video_fifo_, video_cache_;
    QMutex cs_video_fifo_, cs_video_cache_;
    QWaitCondition video_fifo_not_empty_, video_cache_not_empty_;

    typedef std::deque<Pcm*> PCMS;
    PCMS audio_fifo_, audio_cache_;
    QMutex cs_audio_fifo_, cs_audio_cache_;
    QWaitCondition audio_fifo_not_empty_, audio_cache_not_empty_;
};

#endif // MEDIATHREAD_H

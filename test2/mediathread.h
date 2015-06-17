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

/** 启动工作线程，保存解码帧 ...
 */
class MediaThread : public QThread
                  , ffmpegDecoderCallback
{
    Q_OBJECT

public:
    MediaThread(const char *url);
    virtual ~MediaThread();

    class Picture
    {
        double stamp_;
        int width_, height_;
        AVPicture pic_;
        QImage *image_;
        SwsContext *sws_;

    public:
        Picture()
        {
            width_ = height_ = 16;
            sws_ = sws_getContext(width_, height_, AV_PIX_FMT_YUV420P, width_, height_, AV_PIX_FMT_RGB32, SWS_FAST_BILINEAR, 0, 0, 0);
            avpicture_alloc(&pic_, AV_PIX_FMT_RGB32, width_, height_);
            image_ = new QImage(pic_.data[0], width_, height_, pic_.linesize[0], QImage::Format_RGB32);
        }

        ~Picture()
        {
            sws_freeContext(sws_);
            delete image_;
            avpicture_free(&pic_);
        }

        void save(const AVFrame *frame, double stamp)
        {
            this->stamp_ = stamp;

            if (frame->width != width_ || frame->height != height_) {
                width_ = frame->width, height_ = frame->height;

                sws_freeContext(sws_);
                delete image_;
                avpicture_free(&pic_);

                avpicture_alloc(&pic_, AV_PIX_FMT_RGB32, width_, height_);
                image_ = new QImage(pic_.data[0], width_, height_, pic_.linesize[0], QImage::Format_RGB32);
                sws_ = sws_getContext(frame->width, frame->height, (PixelFormat)frame->format,
                                     width_, height_, AV_PIX_FMT_RGB32, SWS_FAST_BILINEAR, 0, 0, 0);
            }

            sws_scale(sws_, frame->data, frame->linesize, 0, frame->height, pic_.data, pic_.linesize);
        }

        QImage *image() const
        {
            return image_;
        }

        double stamp() const
        {
            return stamp_;
        }
    };

    double next_stamp()
    {
        if (has_pending()) {
            return first_pending_stamp();
        }
        else {
            return 0x7ffffff;   // FIXME:
        }
    }

    // 如果有，则返回第一帧...
    Picture *lock_picture()
    {
        if (has_pending()) {
            return next_picture();
        }
        else {
            return 0;
        }
    }

    void unlock_picture(Picture *p)
    {
        save_freed_picture(p);
    }

private:
    virtual void run();
    virtual int on_audio_frame(int idx, const AVFrame *frame, double stamp);
    virtual int on_video_frame(int idx, const AVFrame *frame, double stamp);

    Picture *next_freed_picture()
    {
        cs_cache_.lock();
        while (cache_.empty()) {
            cache_not_empty_.wait(&cs_cache_);
        }

        Picture *p = cache_.front();
        cache_.pop_front();
        cs_cache_.unlock();

        return p;
    }

    static bool op_comp_stamp(Picture * const &p1, Picture * const &p2)
    {
        return p1->stamp() < p2->stamp();
    }

    void save_data_picture(Picture *p)
    {
        cs_fifo_.lock();
        fifo_.push_back(p);
        std::sort(fifo_.begin(), fifo_.end(), op_comp_stamp); // 按照 pts 时间戳排序
        fifo_not_empty_.wakeAll();
        cs_fifo_.unlock();
    }

    Picture *next_picture()
    {
        cs_fifo_.lock();
        while (fifo_.empty()) {
            fifo_not_empty_.wait(&cs_fifo_);
        }

        Picture *p = fifo_.front();
        fifo_.pop_front();
        cs_fifo_.unlock();

        return p;
    }

    bool has_pending()
    {
        cs_fifo_.lock();
        bool empty = fifo_.empty();
        cs_fifo_.unlock();
        return !empty;
    }

    double first_pending_stamp()
    {
        cs_fifo_.lock();
        double stamp = fifo_.front()->stamp();
        cs_fifo_.unlock();
        return stamp;
    }

    void save_freed_picture(Picture *p)
    {
        cs_cache_.lock();
        cache_.push_back(p);
        cache_not_empty_.wakeAll();
        cs_cache_.unlock();
    }

    void prepare_cache(int n)
    {
        for (int i = 0; i < n; i++) {
            cache_.push_back(new Picture);
        }
    }

private:
    bool quit_;
    std::string url_;
    typedef std::deque<Picture*> PICTURES;
    PICTURES fifo_, cache_;
    QMutex cs_fifo_, cs_cache_;
    QWaitCondition fifo_not_empty_, cache_not_empty_;
};

#endif // MEDIATHREAD_H

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include "zkrender.h"
#include "MediaThread.h"
#include "DecodeResult.h"
#include <cc++/thread.h>
#include <deque>
#include <stack>
#include <QImage>
#include <QTimer>
#include <assert.h>
extern "C" {
#   include <libswscale/swscale.h>
}

inline double now()
{
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

class Player : public QQuickPaintedItem, public DecodeResult
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl)

public:
    Player();
    ~Player();

    void setUrl(const QString &url);
    QString url() const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    void paint(QPainter *painter);

private:
    /** MediaThread 回调，这里可以阻塞，以控制解码线程
     */
    virtual int save_video_frame(const AVFrame *origin);

private Q_SLOTS:
    void check_pending();

private:
    /** 缓冲的解码后的图像帧 */
    class SavedPicture
    {
    public:
        SavedPicture()
        {
            width = height = 0;
            image_ = 0;
            sws = 0;
            pic.data[0] = 0;
        }

        ~SavedPicture()
        {
            delete image_;
            if (pic.data[0]) {
                avpicture_free(&pic);
            }
            if (sws) {
                sws_freeContext(sws);
            }
        }

        void save(const AVFrame *frame)
        {
            if (width != frame->width || height != frame->height) {
                width = frame->width, height = frame->height;
                if (sws) {
                    sws_freeContext(sws);
                    sws = sws_getContext(frame->width, frame->height, (PixelFormat)frame->format,
                                         width, height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, 0, 0, 0);
                }

                if (pic.data[0]) {
                    avpicture_free(&pic);
                }

                avpicture_alloc(&pic, AV_PIX_FMT_RGB32, width, height);

                delete image_;
                image_ = new QImage(pic.data[0], width, height, QImage::Format_ARGB32);
            }

            sws_scale(sws, frame->data, frame->linesize, 0, frame->height,
                      pic.data, pic.linesize);

            pts = frame->pts;
        }

        int64_t stamp() const
        {
            return this->pts;
        }

        QImage *image() const
        {
            return image_;
        }

    private:
        SwsContext *sws;
        AVPicture pic;
        int64_t pts;
        int width, height;
        QImage *image_;
    };

    size_t pending_size()
    {
        ost::MutexLock al(cs_fifo_);
        return fifo_.size();
    }

    int64_t pending_next_pts()
    {
        assert(pending_size() > 0);
        ost::MutexLock al(cs_fifo_);
        return fifo_.front()->stamp();
    }

    QImage *pending_next_image()
    {
        assert(pending_size() > 0);
        ost::MutexLock al(cs_fifo_);
        return fifo_.front()->image();
    }

    void release_pending_image()
    {
        assert(pending_size() > 0);
        cs_fifo_.enter();
        SavedPicture *sp = fifo_.front();
        fifo_.pop_front();
        cs_fifo_.leave();

        cs_cache_.enter();
        cache_.push_back(sp);
        evt_cache_.signal();
        cs_cache_.leave();
    }

    // 检查是否需要播放一帧...
    bool chk_to_show();

    typedef std::deque<SavedPicture*> PICTURES;
    PICTURES fifo_, cache_;
    ost::Mutex cs_fifo_, cs_cache_;
    ost::Event evt_fifo_, evt_cache_;

    QTimer timer_; // 每隔10ms检查是否有需要显示的图像帧...
    QImage *image_showing_; //

    QString url_;
    zkrender_t *render_;
    MediaThread *thread_;   // 媒体线程..

    bool waiting_first_frame_;  // 为了同步...
    double pts_delta_;  // now与frame pts的差值 ..
};

#endif // PLAYER_H

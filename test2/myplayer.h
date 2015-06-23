#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QAudioOutput>
#include <QIODevice>
#include <assert.h>
#include "mediathread.h"
#include "circ_buf.h"
#include "kvconfig.h"

extern KVConfig *_cfg;

#define AU_BUFSIZE (64*1024)
class AudioBuffer : public QIODevice
{
    bool first_audio_;  // 一开始，等待缓冲的数据足够多时，再开始填充 ...
    MediaThread *mt_;
    double stamp_audio_delta_;
    char buf_[AU_BUFSIZE];
    int head_, tail_;

public:
    AudioBuffer(MediaThread *mt);

    int idle_size() const { return CIRC_SPACE(head_, tail_, AU_BUFSIZE); }
    int data_size() const { return CIRC_CNT(head_, tail_, AU_BUFSIZE); }

private:
    void append(unsigned char *data, int len)
    {
        assert(idle_size() >= len);

        int se = CIRC_SPACE_TO_END(head_, tail_, AU_BUFSIZE);
        if (se >= len) {
            memcpy(buf_+head_, data, len);
        }
        else {
            memcpy(buf_+head_, data, se);
            memcpy(buf_, data+se, len-se);
        }

        head_ = (head_ + len) & (AU_BUFSIZE-1);
    }

    // 尽量填充本地 buf
    void load_from_mt()
    {
        while (mt_->audio_pending_size() > 0) {
            int bytes = mt_->audio_pending_next_bytes();
            if (idle_size() >= bytes) {
                MediaThread::Pcm *pcm = mt_->lock_pcm();
                int len;
                void *p = pcm->data(len);
                assert(len == bytes);
                append((unsigned char*)p, len);
                mt_->unlock_pcm(pcm);
            }
            else {
                break;
            }
        }
    }

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
};

class MyPlayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool cl_enabled READ cl_enabled WRITE cl_setEnabled NOTIFY cl_enabledChanged)
    Q_PROPERTY(double cache_duration READ cache_duration WRITE setCache_duration NOTIFY cache_durationChanged)

public:
    MyPlayer();
    virtual ~MyPlayer();
    virtual void paint(QPainter *painter);

    QString url() const { return url_; }
    void setUrl(const QString &url) { url_ = url; }

    bool cl_enabled() const { return cl_enabled_; }
    void cl_setEnabled(bool enable) { cl_enabled_ = enable; }

    double cache_duration() const { return duration_; }
    void setCache_duration(double d) { duration_ = d; }


    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void cl_push_point(int x, int y);
    Q_INVOKABLE void cl_pop_point();
    Q_INVOKABLE void cl_remove_all_points();
    Q_INVOKABLE int cl_points();
    Q_INVOKABLE void cl_save();

private slots:
    void when_check_frame();
    void when_cl_enabledChanged();
    void when_cache_durationChanged();

signals:
    void urlChanged();
    void cl_enabledChanged();
    void cache_durationChanged();

private:
    void check_video_frame(double now);
    void load_calibration_data();

private:
    QString url_, info_;
    QTimer timer_;
    MediaThread *th_;
    MediaThread::Picture *img_rending_;
    bool first_video_, first_audio_;
    double stamp_video_delta_, stamp_audio_delta_;
    QAudioOutput *audio_output_;
    AudioBuffer *ab_;

    bool cl_enabled_;
    std::deque<QPoint> cl_points_;

    double duration_;

    KVConfig *cfg_;
};

#endif // MYPLAYER_H

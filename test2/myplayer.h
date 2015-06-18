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

class AudioBuffer : public QIODevice
{
#define BUFSIZE (64*1024)
    MediaThread *mt_;
    bool first_audio_;
    double stamp_audio_delta_;
    char buf_[BUFSIZE];
    int head_, tail_;

public:
    AudioBuffer(MediaThread *mt);
    void append(unsigned char *data, int len)
    {
        assert(idle_size() >= len);
        int se = CIRC_SPACE_TO_END(head_, tail_, BUFSIZE);
        if (se >= len) {
            memcpy(buf_+head_, data, len);
        }
        else {
            memcpy(buf_+head_, data, se);
            memcpy(buf_, data+se, len-se);
        }

        head_ += len;
        head_ %= BUFSIZ;
    }

    int idle_size() const { return CIRC_SPACE(head_, tail_, BUFSIZE); }
    int data_size() const { return CIRC_SPACE(head_, tail_, BUFSIZE); }


private:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
};

class MyPlayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

public:
    MyPlayer();
    virtual ~MyPlayer();
    virtual void paint(QPainter *painter);

    QString url() const { return url_; }
    void setUrl(const QString &url) { url_ = url; }

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

private slots:
    void check_frame();

signals:
    void urlChanged();

private:
    void check_audio_frame(double now);
    void check_video_frame(double now);

private:
    QString url_, info_;
    QTimer timer_;
    MediaThread *th_;
    MediaThread::Picture *img_rending_;
    bool first_video_, first_audio_;
    double stamp_video_delta_, stamp_audio_delta_;
    QAudioOutput *audio_output_;
    AudioBuffer *ab_;
};

#endif // MYPLAYER_H

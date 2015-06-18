#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QAudioOutput>
#include <QIODevice>
#include "mediathread.h"

class AudioBuffer : public QIODevice
{
    MediaThread *mt_;
public:
    AudioBuffer(MediaThread *mt);

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
    void playback_audio(MediaThread::Pcm *pcm);

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

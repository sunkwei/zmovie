#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include "mediathread.h"

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
    QString url_;
    QTimer timer_;
    MediaThread *th_;
    QImage *img_rending_;
};

#endif // MYPLAYER_H

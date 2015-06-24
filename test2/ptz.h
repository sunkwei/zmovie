#ifndef PTZ_H
#define PTZ_H

#include <QQuickItem>
#include <QString>
#include "ptz/ptz0.h"

class Ptz : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl)

    QString url_;
    ptz_t *ptz_;

public:
    Ptz();

    QString url() const { return url_; }
    void setUrl(const QString &url)
    {
        if (ptz_) {
            ptz_close(ptz_);
            ptz_ = 0;
        }

        url_ = url;
        if (!url_.isEmpty()) {
            ptz_ = ptz_open(url_.toStdString().c_str(), 0);
        }
    }

    Q_INVOKABLE bool left(int speed);
    Q_INVOKABLE bool right(int speed);
    Q_INVOKABLE bool up(int speed);
    Q_INVOKABLE bool down(int speed);
    Q_INVOKABLE bool stop();

    Q_INVOKABLE bool preset_save(int id);
    Q_INVOKABLE bool preset_call(int id);
    Q_INVOKABLE bool preset_clear(int id);

    Q_INVOKABLE bool set_pos(int x, int y, int speedx = 32, int speedy = 32);
    Q_INVOKABLE bool set_zoom(int z);

    Q_INVOKABLE bool zoom_tele(int speed);
    Q_INVOKABLE bool zoom_wide(int speed);
    Q_INVOKABLE bool zoom_stop();

    /** in QML, just using:
     *      pos = ptz.get_pos();
     *      if (pos.err == 0) {
     *          x = pos.x;
     *          y = pos.y;
     *      }
     */
    Q_INVOKABLE QVariant get_pos();

    /** in QML, just using:
     *      zoom = ptz.get_zoom();
     *      if (zoom.err == 0) {
     *          z = pos.zoom;
     *      }
     */
    Q_INVOKABLE QVariant get_zoom();

public slots:
};

#endif // PTZ_H

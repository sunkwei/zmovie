#define Player1

#include <cc++/socket.h>
#include <QQuickWindow>
#include "zkrender.h"
#include <QPainter>
#include "player.h"
#include <QDebug>

Player::Player()
{
    thread_ = 0;
    image_showing_ = 0;

    for (int i = 0; i < 10; i++) {
        cache_.push_back(new SavedPicture);
    }

    timer_.setInterval(10);
    QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(check_pending()));
}

Player::~Player()
{
    if (1) {
        ost::MutexLock al(cs_fifo_);
        for (PICTURES::iterator it = fifo_.begin(); it != fifo_.end(); ++it) {
            delete *it;
        }
        fifo_.clear();
    }

    if (1) {
        ost::MutexLock al(cs_cache_);
        for (PICTURES::iterator it = cache_.begin(); it != cache_.end(); ++it) {
            delete *it;
        }
        cache_.clear();
    }
}

void Player::setUrl(const QString &url)
{
    url_ = url;
}

QString Player::url() const
{
    return url_;
}

void Player::play()
{
    waiting_first_frame_ = true;
    thread_ = new MediaThread(url_.toStdString().c_str(), this);
    image_showing_ = 0;
    timer_.start();
}

void Player::stop()
{
    timer_.stop();

    delete thread_;
    thread_ = 0;
}

void Player::paint(QPainter *painter)
{
    if (image_showing_) {
        QImage *img = pending_next_image();
        QRectF target(0, 0, width(), height());
        QRectF src(0, 0, img->width(), img->height());
        painter->drawImage(target, *img, src);
        release_pending_image();
        qDebug("X");
    }

    image_showing_ = 0;
}

void Player::check_pending()
{
    if (chk_to_show()) {
        image_showing_ = pending_next_image();
        update();
    }
}

bool Player::chk_to_show()
{
#if 0
    if (waiting_first_frame_) {
        if (pending_size() < 5) {
            return false;
        }

        pts_delta_ = now() - pending_next_pts();
        waiting_first_frame_ = false;
    }

    if (pending_size() == 0) {
        return false;
    }

    double fpts = pending_next_pts();
    double curr = now() - pts_delta_;

    if (fpts <  curr) {
        return true;
    }
    else {
        return false;
    }
#else
    if (pending_size() > 0) {
        return true;
    }
#endif //
}

int Player::save_video_frame(const AVFrame *origin)
{
    cs_cache_.enter();
    while (cache_.empty()) {
        cs_cache_.leave();
        evt_cache_.wait();
        evt_cache_.reset();

        // TODO: check quit
    }

    SavedPicture *sp = cache_.front();
    cache_.pop_front();
    cs_cache_.leave();

    sp->save(origin);

    cs_fifo_.enter();
    fifo_.push_back(sp);
    evt_fifo_.signal();
    cs_fifo_.leave();

    qDebug("V");

    return 0;
}

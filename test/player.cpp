#include "player.h"
#include <QQuickWindow>
#include "zkrender.h"
#include <QPainter>

Player::Player()
{
    thread_ = 0;

    // 准备10个缓冲....
    for (int i = 0; i < 10; i++) {
        cache_.push_back(new SavedPicture);
    }

    // Timer, 每隔10ms，检查 fifo_ 中第一帧图像的 pts，...
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
    // 启动工作线程，接收媒体数据，解码，保存到 cache_，在主线程中检查是否需要显示 ...
    waiting_first_frame_ = true;
    thread_ = new MediaThread(url_.toStdString().c_str(), this);
    image_showing_ = false;
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
        // 显示
        painter->drawImage(0, 0, *pending_next_image());
        release_pending_image();
    }
}

void Player::check_pending()
{
    /** 检查 fifo_ 中的帧是否需要显示，是否过时 ....
     */
    if (chk_to_show()) {
        image_showing_ = pending_next_image();
        update();
    }
}

bool Player::chk_to_show()
{
    if (waiting_first_frame_) {
        // 至少缓冲5帧再开始播放..
        if (pending_size() < 5) {
            return false;
        }

        pts_delta_ = now() - pending_next_pts();
    }

    if (pending_size() == 0) {
        return false;
    }

    double fpts = pending_next_pts();
    double curr = now() - pts_delta_;

    // TODO: 如果cpu能力不够，则会出现累积，需要主动扔帧 ...

    if (fpts <  curr) {
        return true;
    }
    else {
        return false;
    }
}


int Player::save_video_frame(const AVFrame *origin)
{
    /** 保存到 QImage 中，并放入 fifo_, 在必要的时候显示 */
    cs_cache_.enter();
    while (cache_.empty()) {
        cs_cache_.leave();
        evt_cache_.wait();
        evt_cache_.reset();

        /** TODO: 这里需要检查是否结束 ... */
    }

    SavedPicture *sp = cache_.front();
    cache_.pop_front();
    cs_cache_.leave();

    sp->save(origin);

    cs_fifo_.enter();
    fifo_.push_back(sp);
    cs_fifo_.leave();

    return 0;
}

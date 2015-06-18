#include "myplayer.h"
#include <QPainter>

MyPlayer::MyPlayer()
{
    img_rending_ = 0;
    timer_.setInterval(10); // 每隔10ms，查询是否需要刷新 ...
    QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(check_frame()));

    th_ = 0;
}

MyPlayer::~MyPlayer()
{
    stop();
}

void MyPlayer::check_frame()
{
    // 每隔10ms，检查是否有需要 render 的图像 ...
    assert(th_);

    if (first_video_) {
        // 等待至少缓冲几帧之后，再开始真正的播放，这样能够更平滑 ...
        if (th_->pending_duration() > 0.3) {
            first_video_ = false;
            stamp_video_delta_ = util_now() - th_->pending_first_stamp();
            qDebug("pending duration %.3f, size=%u", th_->pending_duration(), th_->pending_size());
        }
        else {
            return;
        }
    }

    if (th_->pending_size() == 0) {
        // 说明似乎是播放的速度太快了，需要重新等待缓冲满???
        first_video_ = true;
    }
    else {
        // 检查是否已经 painter 完成 ...
        if (!img_rending_) {
            // 检查是否此时应该播放该帧 ...
            double stamp = th_->pending_first_stamp();
            if (util_now() - stamp_video_delta_ > stamp) {
                qDebug("[%.3f]: R: pending size=%u", util_now() - stamp_video_delta_, th_->pending_size());

                MediaThread::Picture *p = th_->lock_picture();
                img_rending_ = p;
                update();
            }
        }
        else {
            qDebug("rending op tooo slow !!!");
        }
    }
}

void MyPlayer::paint(QPainter *painter)
{
    if (img_rending_) {
        QRectF r(0, 0, width(), height());
        painter->drawImage(r, *img_rending_->image());
        th_->unlock_picture(img_rending_);
        img_rending_ = 0;
    }
    else {
        if (info_.isEmpty()) {
            painter->drawText(0, 50, url_);
        }
        else {
            painter->drawText(10, 50, info_);
        }
    }
}

void MyPlayer::play()
{
    if (!th_) {
        first_video_ = true;
        timer_.start();
        th_ = new MediaThread(url_.toStdString().c_str());
        info_ = "loading ..." + url_;
        update();
    }
}

void MyPlayer::stop()
{
    if (th_) {
        timer_.stop();

        delete th_;
        th_ = 0;

        info_ = "";
        update();
    }
}

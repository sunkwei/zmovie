#include "myplayer.h"
#include <QPainter>

MyPlayer::MyPlayer()
{
    img_rending_ = 0;
    timer_.setInterval(10);
    QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(check_frame()));
    timer_.start();

    th_ = 0;
}

MyPlayer::~MyPlayer()
{
    delete th_;
    timer_.stop();
}

void MyPlayer::check_frame()
{
    // 每隔10ms，检查是否有需要 render 的图像 ...
    if (th_) {
        /** 应该根据时间戳，决定是否 update
         *      double stamp = th_->next_stamp();
         *      if (stamp < now) {
         *          update()
         *      }
         */

        MediaThread::Picture *p = th_->lock_picture();
        if (p) {
            img_rending_ = p->image();
            update();
            th_->unlock_picture(p);
        }
    }
}

void MyPlayer::paint(QPainter *painter)
{
    if (img_rending_) {
        QRectF r(0, 0, width(), height());
        painter->drawImage(r, *img_rending_);
        img_rending_ = 0;
    }
    else {
        painter->drawText(0, 50, url_);
    }
}

void MyPlayer::play()
{
    th_ = new MediaThread(url_.toStdString().c_str());
}

void MyPlayer::stop()
{
    delete th_;
    th_ = 0;
}

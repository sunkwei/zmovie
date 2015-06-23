#include "myplayer.h"
#include <QPainter>
#include <stdio.h>
#include <QRect>
#include <QPen>

MyPlayer::MyPlayer()
{
    cfg_ = _cfg;

    img_rending_ = 0;
    timer_.setInterval(10);
    QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(when_check_frame()));
    QObject::connect(this, SIGNAL(cl_enabledChanged()), this, SLOT(when_cl_enabledChanged()));

    th_ = 0;

    QAudioFormat fmt;
    fmt.setChannelCount(2);
    fmt.setSampleRate(32000);
    fmt.setSampleSize(16);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::SignedInt);

    audio_output_ = new QAudioOutput(fmt, this);
    ab_ = 0;
    cl_enabled_ = false;

    load_calibration_data();
}

MyPlayer::~MyPlayer()
{
    stop();
    delete audio_output_;
}

void MyPlayer::when_check_frame()
{
    double now = util_now();
    check_video_frame(now);
}

void MyPlayer::check_video_frame(double now)
{
    // 每隔10ms，检查是否有需要 render 的图像 ...
    assert(th_);

    if (first_video_) {
        // 等待至少缓冲几帧之后，再开始真正的播放，这样能够更平滑 ...
        if (th_->video_pending_duration() > 0.3) {
            first_video_ = false;
            stamp_video_delta_ = now - th_->video_pending_first_stamp();
            qDebug("pending video duration %.3f, size=%u", th_->video_pending_duration(), th_->video_pending_size());
        }
        else {
            return;
        }
    }

    if (th_->video_pending_size() == 0) {
        // 说明似乎是播放的速度太快了，需要重新等待缓冲满???
        first_video_ = true;
        qDebug("video underflow ....");
    }
    else {
        // 检查是否已经 painter 完成 ...
        if (!img_rending_) {
            // 检查是否此时应该播放该帧 ...
            double stamp = th_->video_pending_first_stamp();
            if (now - stamp_video_delta_ > stamp) {
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
    if (img_rending_ && th_) {
        QRectF r(0, 0, width(), height());
        QImage *img = img_rending_->image();

        // 画标定线 ...
        if (cl_enabled() && !cl_points_.empty()) {
            QPen pen(QColor(0, 255, 0)), pent(QColor(255, 0, 0));
            pen.setWidth(3), pent.setWidth(3);

            QPainter p;
            p.begin(img);
            std::deque<QPoint>::const_iterator it = cl_points_.begin(), it0 = it;

            int x0 = it0->x() * img->width() / width(), x = x0;
            int y0 = it0->y() * img->height() / height(), y = y0;

            p.setPen(pen);
            p.drawRect(x0-5, y0-5, 10, 10);

            for (++it; it != cl_points_.end(); ++it, ++it0) {
                int x = it->x() * img->width() / width();
                int y = it->y() * img->height() / height();

                p.drawLine(QPoint(x0, y0), QPoint(x, y));

                x0 = x, y0 = y;
            }

            if (x != x0 || y != y0) {
                p.setPen(pent);
                p.drawLine(QPoint(x, y), QPoint(x0, y0));
            }

            p.end();
        }

        painter->drawImage(r, *img);
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
        first_audio_ = true;
        first_video_ = true;

        timer_.start();

        th_ = new MediaThread(url_.toStdString().c_str());

        ab_ = new AudioBuffer(th_);
        bool rc = ab_->open(QIODevice::ReadOnly);
        audio_output_->start(ab_);

        info_ = "loading ..." + url_;
        update();
    }
}

void MyPlayer::stop()
{
    if (th_) {
        audio_output_->stop();

        timer_.stop();

        if (img_rending_) {
            th_->unlock_picture(img_rending_);
            img_rending_ = 0;
        }

        delete th_;
        th_ = 0;

        info_ = "";
        update();
    }
}

AudioBuffer::AudioBuffer(MediaThread *mt)
    : mt_(mt)
{
    head_ = tail_ = 0;
    first_audio_ = true;
}

static qint64 silence(char *data, qint64 maxlen)
{
    int n = maxlen > 1024 ? 1024 : maxlen;
    memset(data, 0, n);
    return n;
}

qint64 AudioBuffer::readData(char *data, qint64 maxlen)
{
    // TODO: 这里根据音频数据的消耗，可以计算音频时间戳，并用于视频同步 ...

    load_from_mt(); // 总是尽量填充

    if (first_audio_) {
        // 多积累点儿声音数据，再开始投递 ...
        if (data_size() > 16*1024) {
            first_audio_ = false;
        }
        else {
            return silence(data, maxlen);
        }
    }

    if (maxlen <= 0) {
        return maxlen;
    }

    int bytes = maxlen <= data_size() ? maxlen : data_size();
    if (bytes < 512) {
        first_audio_ = true;
        qDebug("silence ...");

        // 返回静音数据 ...
        return silence(data, maxlen);
    }

    int de = CIRC_CNT_TO_END(head_, tail_, AU_BUFSIZE);
    if (de >= bytes) {
        memcpy(data, buf_+tail_, bytes);
    }
    else {
        memcpy(data, buf_+tail_, de);
        memcpy(data+de, buf_, bytes-de);
    }
    tail_ = (tail_+bytes) & (AU_BUFSIZE-1);

    return bytes;
}

qint64 AudioBuffer::writeData(const char *data, qint64 len)
{
    assert(0);
    return 0;
}

void MyPlayer::load_calibration_data()
{
    char *p = ::strdup(cfg_->get_value("calibration_data", ""));
    char *q = strtok(p, ";");
    while (q) {
        int x, y;
        if (sscanf(q, "%d,%d", &x, &y) == 2) {
            cl_points_.push_back(QPoint(x, y));
        }

        q = strtok(0, ";");
    }
    free(p);
}

void MyPlayer::when_cl_enabledChanged()
{
    if (cl_enabled()) {

    }
    else {

    }
}

int MyPlayer::cl_points()
{
    return cl_points_.size();
}

void MyPlayer::cl_push_point(int x, int y)
{
    if (cl_enabled()) {
        cl_points_.push_back(QPoint(x, y));
    }
}

void MyPlayer::cl_pop_point()
{
    if (cl_enabled() && !cl_points_.empty()) {
        cl_points_.pop_back();
    }
}

void MyPlayer::cl_remove_all_points()
{
    cl_points_.clear();
}

void MyPlayer::cl_save()
{
    if (cl_enabled()) {
        std::stringstream ss;
        for (int i = 0; i < cl_points_.size(); i++) {
            int x = cl_points_[i].x() * atoi(cfg_->get_value("video_width", "480")) / width();
            int y = cl_points_[i].y() * atoi(cfg_->get_value("video_height", "270")) / height();
            ss << x << ',' << y << ';';
        }

        std::string cd = ss.str();

        cfg_->set_value("calibration_data", cd.c_str());
        cfg_->save_as();
    }
}

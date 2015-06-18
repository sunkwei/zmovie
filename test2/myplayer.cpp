#include "myplayer.h"
#include <QPainter>

MyPlayer::MyPlayer()
{
    img_rending_ = 0;
    timer_.setInterval(10); // 每隔10ms，查询是否需要刷新 ...
    QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(check_frame()));

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
}

MyPlayer::~MyPlayer()
{
    stop();
    delete audio_output_;
}

void MyPlayer::check_frame()
{
    double now = util_now();
//    check_audio_frame(now);
    check_video_frame(now);
}

void MyPlayer::check_audio_frame(double now)
{
    // 每隔10毫秒，检查是否需要投递的声音 pcm ...
    if (first_audio_) {
        // 等待至少缓冲几帧声音之后，再开始 ...
        if (th_->audio_pending_duration() > 0.3) {
            first_audio_ = false;
            stamp_audio_delta_ = now - th_->audio_pending_first_stamp();
            qDebug("pending audio duration:%.3f, size:%u", th_->audio_pending_duration(), th_->audio_pending_size());
        }
        else {
            return;
        }
    }

    if (th_->audio_pending_size() == 0) {
        //
        first_audio_ = true;
        qDebug("audio underflow ...");
    }
    else {
        double stamp = th_->audio_pending_first_stamp();
        if (now - stamp_audio_delta_ > stamp) {
            MediaThread::Pcm *p = th_->lock_pcm();
            playback_audio(p);
            th_->unlock_pcm(p);
        }
    }
    assert(th_);
    if (th_->audio_pending_size() > 2) {
        MediaThread::Pcm *pcm = th_->lock_pcm();
        if (pcm) {
            // playback;
            th_->unlock_pcm(pcm);
        }
    }
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

void MyPlayer::playback_audio(MediaThread::Pcm *pcm)
{
    if (!audio_output_) {


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
        first_audio_ = true;
        first_video_ = true;

        timer_.start();

        th_ = new MediaThread(url_.toStdString().c_str());

        ab_ = new AudioBuffer(th_);
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

        delete th_;
        th_ = 0;

        info_ = "";
        update();
    }
}

AudioBuffer::AudioBuffer(MediaThread *mt)
    : mt_(mt)
{

}

qint64 AudioBuffer::readData(char *data, qint64 maxlen)
{
    return 0;
}

qint64 AudioBuffer::writeData(const char *data, qint64 len)
{
    assert(0);
    return 0;
}

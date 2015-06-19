#include "mediathread.h"

MediaThread::MediaThread(const char *url)
    : url_(url)
{
    quit_ = false;
    prepare_cache(30);
    start();
}

MediaThread::~MediaThread()
{
    quit_ = true;
    video_fifo_not_empty_.wakeAll();
    video_cache_not_empty_.wakeAll();
    audio_fifo_not_empty_.wakeAll();
    audio_cache_not_empty_.wakeAll();
    wait();
    release_all_buf();
}

void MediaThread::run()
{
    ffmpegWrap fr(url_.c_str(), this);
    while (!quit_) {
        int rc = fr.run_once();
        if (rc != ffmpegWrap::RC_OK) {
            // TODO: 等待下一步 ...
            while (!quit_) {
                msleep(30);
            }
        }
    }

    qDebug("mediaThread terminated!");
}

MediaThread::Picture::Picture()
{
    width_ = height_ = 16;
    sws_ = sws_getContext(width_, height_, AV_PIX_FMT_YUV420P, width_, height_, AV_PIX_FMT_RGB32, SWS_FAST_BILINEAR, 0, 0, 0);
    avpicture_alloc(&pic_, AV_PIX_FMT_RGB32, width_, height_);
    image_ = new QImage(pic_.data[0], width_, height_, pic_.linesize[0], QImage::Format_RGB32);
}

MediaThread::Picture::~Picture()
{
    sws_freeContext(sws_);
    delete image_;
    avpicture_free(&pic_);
}

void MediaThread::Picture::save(const AVFrame *frame, double stamp)
{
    this->stamp_ = stamp;

    if (frame->width != width_ || frame->height != height_) {
        width_ = frame->width, height_ = frame->height;

        sws_freeContext(sws_);
        delete image_;
        avpicture_free(&pic_);

        avpicture_alloc(&pic_, AV_PIX_FMT_RGB32, width_, height_);
        image_ = new QImage(pic_.data[0], width_, height_, pic_.linesize[0], QImage::Format_RGB32);
        sws_ = sws_getContext(frame->width, frame->height, (PixelFormat)frame->format,
                             width_, height_, AV_PIX_FMT_RGB32, SWS_FAST_BILINEAR, 0, 0, 0);
    }

    sws_scale(sws_, frame->data, frame->linesize, 0, frame->height, pic_.data, pic_.linesize);
}

QImage *MediaThread::Picture::image() const
{
    return image_;
}

double MediaThread::Picture::stamp() const
{
    return stamp_;
}

size_t MediaThread::video_pending_size()
{
    cs_video_fifo_.lock();
    size_t s = video_fifo_.size();
    cs_video_fifo_.unlock();
    return s;
}

double MediaThread::video_pending_duration()
{
    double duration = 0.0;
    cs_video_fifo_.lock();
    if (video_fifo_.size() > 1) {
        duration = video_fifo_.back()->stamp() - video_fifo_.front()->stamp();
    }
    cs_video_fifo_.unlock();
    return duration;
}

double MediaThread::video_pending_first_stamp()
{
    cs_video_fifo_.lock();
    assert(!video_fifo_.empty());
    double stamp = video_fifo_.front()->stamp();
    cs_video_fifo_.unlock();
    return stamp;
}

MediaThread::Picture *MediaThread::lock_picture()
{
    cs_video_fifo_.lock();
    assert(!video_fifo_.empty());
    Picture *p = video_fifo_.front();
    video_fifo_.pop_front();
    cs_video_fifo_.unlock();
    return p;
}

void MediaThread::unlock_picture(MediaThread::Picture *p)
{
    save_freed_picture(p);
}

MediaThread::Picture *MediaThread::next_freed_picture()
{
    cs_video_cache_.lock();
    while (video_cache_.empty() && !quit_) {
        video_cache_not_empty_.wait(&cs_video_cache_);
    }

    if (quit_) {
        cs_video_cache_.unlock();
        return 0;
    }

    Picture *p = video_cache_.front();
    video_cache_.pop_front();
    cs_video_cache_.unlock();

    return p;
}

MediaThread::Pcm *MediaThread::next_freed_pcm()
{
    cs_audio_cache_.lock();
    while (audio_cache_.empty() && !quit_) {
        audio_cache_not_empty_.wait(&cs_audio_cache_);
    }

    if (quit_) {
        cs_audio_cache_.unlock();
        return 0;
    }

    Pcm *p = audio_cache_.front();
    audio_cache_.pop_front();
    cs_audio_cache_.unlock();

    return p;
}

static bool op_comp_stamp(MediaThread::Picture * const &p1, MediaThread::Picture * const &p2)
{
    return p1->stamp() < p2->stamp();
}

void MediaThread::save_data_pcm(MediaThread::Pcm *p)
{
    cs_audio_fifo_.lock();
    audio_fifo_.push_back(p);
    audio_fifo_not_empty_.wakeAll();
    cs_audio_fifo_.unlock();
}

void MediaThread::save_data_picture(MediaThread::Picture *p)
{
    cs_video_fifo_.lock();
    video_fifo_.push_back(p);
    std::sort(video_fifo_.begin(), video_fifo_.end(), op_comp_stamp);
    video_fifo_not_empty_.wakeAll();
    cs_video_fifo_.unlock();
}

MediaThread::Picture *MediaThread::next_picture()
{
    cs_video_fifo_.lock();
    while (video_fifo_.empty() && !quit_) {
        video_fifo_not_empty_.wait(&cs_video_fifo_);
    }

    if (quit_) {
        cs_video_fifo_.unlock();
        return 0;
    }

    Picture *p = video_fifo_.front();
    video_fifo_.pop_front();
    cs_video_fifo_.unlock();

    return p;
}

void MediaThread::save_freed_picture(MediaThread::Picture *p)
{
    cs_video_cache_.lock();
    video_cache_.push_back(p);
    video_cache_not_empty_.wakeAll();
    cs_video_cache_.unlock();
}

void MediaThread::prepare_cache(int n)
{
    for (int i = 0; i < n; i++) {
        video_cache_.push_back(new Picture);
        audio_cache_.push_back(new Pcm);
    }
}

void MediaThread::release_all_buf()
{
    for (PICTURES::iterator it = video_fifo_.begin(); it != video_fifo_.end(); ++it) {
        delete *it;
    }
    video_fifo_.clear();

    for (PICTURES::iterator it = video_cache_.begin(); it != video_cache_.end(); ++it) {
        delete *it;
    }
    video_cache_.clear();

    for (PCMS::iterator it = audio_cache_.begin(); it != audio_cache_.end(); ++it) {
        delete *it;
    }
    audio_cache_.clear();

    for (PCMS::iterator it = audio_fifo_.begin(); it != audio_fifo_.end(); ++it) {
        delete *it;
    }
    audio_fifo_.clear();
}

MediaThread::Pcm::Pcm()
{
    buf_len_ = 16 * 1024;
    buf_ = (unsigned char*)malloc(buf_len_);
    data_len_ = 0;
    swr_ = 0;
}

MediaThread::Pcm::~Pcm()
{
    free(buf_);
    if (swr_) {
        swr_free(&swr_);
    }
}

void *MediaThread::Pcm::data(int &len) const
{
    len = data_len_;
    return buf_;
}

double MediaThread::Pcm::stamp() const
{
    return stamp_;
}

void MediaThread::Pcm::save(const AVFrame *frame, double stamp)
{
    stamp_ = stamp;

#if 0
    memset(buf_, 0, 4096);
    data_len_ = 4096;
#else
    if (!swr_) {
        // 总是输出 2, s16, 32000 ...
        swr_ = swr_alloc_set_opts(0, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 32000,
                                  frame->channel_layout, (AVSampleFormat)frame->format, frame->sample_rate,
                                  0, 0);

        ch_ = 2;
        samplerate_ = 32000;
        bitsize_ = 16;

        swr_init(swr_);
    }

    size_t out_size = frame->nb_samples * 2 * 2;    // samples * bytes per sample * channels
    if (buf_len_ < out_size) {
        buf_ = (unsigned char*)realloc(buf_, out_size);
        buf_len_ = out_size;
    }

    int samples = swr_convert(swr_, &buf_, frame->nb_samples, (const uint8_t**)frame->extended_data, frame->nb_samples);
    data_len_ = samples * 2 * 2;    // samples * bytes per sample * channels

#endif
}

size_t MediaThread::audio_pending_size()
{
    cs_audio_fifo_.lock();
    size_t s = audio_fifo_.size();
    cs_audio_fifo_.unlock();
    return s;
}

double MediaThread::audio_pending_duration()
{
    double d = 0.0;
    cs_audio_fifo_.lock();
    if (audio_fifo_.size() > 1) {
        d = audio_fifo_.back()->stamp() - audio_fifo_.front()->stamp();
    }
    cs_audio_fifo_.unlock();
    return d;
}

double MediaThread::audio_pending_first_stamp()
{
    cs_audio_fifo_.lock();
    assert(!audio_fifo_.empty());
    double s = audio_fifo_.front()->stamp();
    cs_audio_fifo_.unlock();
    return s;
}

int MediaThread::audio_pending_next_bytes()
{
    cs_audio_fifo_.lock();
    assert(!audio_fifo_.empty());
    int len;
    audio_fifo_.front()->data(len);
    cs_audio_fifo_.unlock();
    return len;
}

MediaThread::Pcm *MediaThread::lock_pcm()
{
    cs_audio_fifo_.lock();
    assert(!audio_fifo_.empty());
    Pcm *p = audio_fifo_.front();
    audio_fifo_.pop_front();
    cs_audio_fifo_.unlock();
    return p;
}

void MediaThread::unlock_pcm(MediaThread::Pcm *pcm)
{
    cs_audio_cache_.lock();
    audio_cache_.push_back(pcm);
    audio_cache_not_empty_.wakeAll();
    cs_audio_cache_.unlock();
}

int MediaThread::on_audio_frame(int idx, const AVFrame *frame, double stamp)
{
    Pcm *p = next_freed_pcm();
    if (p) {
        p->save(frame, stamp);
        save_data_pcm(p);
    }
    return 0;
}

int MediaThread::on_video_frame(int idx, const AVFrame *frame, double stamp)
{
    Picture *p = next_freed_picture();
    if (p) {
        p->save(frame, stamp);
        save_data_picture(p);
    }
    return 0;
}

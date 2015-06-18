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
    fifo_not_empty_.wakeAll();
    cache_not_empty_.wakeAll();
    wait();
    release_all_picture();
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

int MediaThread::on_audio_frame(int idx, const AVFrame *frame, double stamp)
{
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

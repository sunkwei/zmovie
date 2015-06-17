#include "mediathread.h"

MediaThread::MediaThread(const char *url)
    : url_(url)
{
    quit_ = false;
    prepare_cache(10);
    start();
}

MediaThread::~MediaThread()
{
    quit_ = true;
    wait();
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
}

int MediaThread::on_audio_frame(int idx, const AVFrame *frame, double stamp)
{
    qDebug("audio: idx=%d", idx);
    return 0;
}

int MediaThread::on_video_frame(int idx, const AVFrame *frame, double stamp)
{
    qDebug("video: idx=%d, pts=%.3f", idx, stamp);

    Picture *p = next_freed_picture();
    p->save(frame, stamp);
    save_data_picture(p);
    
    return 0;
}

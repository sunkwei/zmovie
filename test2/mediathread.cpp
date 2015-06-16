#include "mediathread.h"

MediaThread::MediaThread(const char *url, DecodeResult *dr)
    : url_(url)
    , dr_(dr)
{
    quit_ = false;
    start();
}

void MediaThread::run()
{
    quit_ = true;
    wait();
}

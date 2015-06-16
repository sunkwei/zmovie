#ifndef MEDIATHREAD_H
#define MEDIATHREAD_H

#include <QThread>
#include <string>
#include "decode_result.h"
extern "C" {
#   include <libavcodec/avcodec.h>
#   include <libavformat/avformat.h>
}

class MediaThread : public QThread
{
    Q_OBJECT
public:
    MediaThread(const char *url, DecodeResult *dr);

private:
    virtual void run();

private:
    bool quit_;
    std::string url_;
    DecodeResult *dr_;
};

#endif // MEDIATHREAD_H

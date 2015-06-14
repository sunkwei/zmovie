#pragma once
#include "WorkingThread.h"
#include "DecodeResult.h"
#include <string>
extern "C" {
#	include <libavformat/avformat.h>
#	include <libswscale/swscale.h>
}

class MediaThread : public Thread
{
	std::string url_;
	DecodeResult *dr_;

	AVFormatContext *ic_;

public:
	MediaThread(const char *url, DecodeResult *dr);
	~MediaThread(void);

private:
	void run();
	int decode_frame(AVFrame *frame, AVPacket *pkg);
};

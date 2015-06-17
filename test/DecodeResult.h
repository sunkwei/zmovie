#pragma once

extern "C" {
#	include <libavcodec/avcodec.h>
}

struct DecodeResult
{
	virtual int save_video_frame(const AVFrame *origin) = 0;
};

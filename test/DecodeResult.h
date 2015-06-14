#pragma once

extern "C" {
#	include <libavcodec/avcodec.h>
#	include <SDL2/SDL.h>
}

struct DecodeResult
{
	virtual int save_video_frame(const AVFrame *origin) = 0;
};

#pragma once

extern "C" {
#	include <libavcodec/avcodec.h>
#	include <sdl2/SDL.h>
}

struct DecodeResult
{
	/// ������Ƶ�󣬱���ͼ�� ....
	virtual int save_video_frame(const AVFrame *origin) = 0;
};

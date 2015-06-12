#pragma once

extern "C" {
#	include <libavcodec/avcodec.h>
#	include <sdl2/SDL.h>
}

struct DecodeResult
{
	/// 解码视频后，保存图像 ....
	virtual int save_video_frame(const AVFrame *origin) = 0;
};

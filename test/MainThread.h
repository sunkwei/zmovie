#pragma once

#include <cc++/thread.h>
#include <SDL2/SDL.h>
#include "WorkingThread.h"
#include "DecodeResult.h"
#include "MediaThread.h"
#include <stack>
#include <deque>
#include <vector>

class MainThread : Thread
				 , public DecodeResult
{
	enum {
		CODE_QUIT = 0,

		CODE_OPEN,
		CODE_CLOSE,

		CODE_VIDEO_CHANGED,
	};

	struct ParamsOpen
	{
		const char *url;
		void *wnd;
	};

	struct CachedFrame
	{
		int64_t pts;
		int width, height;
		AVPicture pic;
	};

    int x_, y_, width_, height_;

    std::deque<CachedFrame *> fifo_;
	std::stack<CachedFrame *> cached_;
	ost::Mutex cs_cached_, cs_fifo_;
	ost::Event evt_cached_, evt_fifo_;

	SDL_Window *win_;
	SDL_Renderer *render_;
	SDL_Texture *texture_;

	MediaThread *media_;

	int video_width_, video_height_;

    std::vector<SDL_Point> points_;
	ost::Mutex cs_points_;

public:
	MainThread(void);
	~MainThread(void);

	int open_with_wnd(const char *url, void *wnd)
	{
		ParamsOpen po;
		po.url = url;
		po.wnd = wnd;

		req(CODE_OPEN, &po);

		return 0;
	}
	void close();

    void set_window_rect(int x, int y, int width, int height);

	void update_points(const std::vector<SDL_Point> &points)
	{
		ost::MutexLock al(cs_points_);
		points_ = points;
	}

private:
	void run();
	void event_loop();
	void frame_refresh(double now);
	void do_req(int code, void *input);

	int open_with_wnd_internal(ParamsOpen *po);
	int video_changed_internal(AVFrame *f);

	int save_video_frame(const AVFrame *origin);

	CachedFrame *lock_video_cache(const AVFrame *f);
	void unlock_video_cache(CachedFrame *p);
	void draw_others();
};

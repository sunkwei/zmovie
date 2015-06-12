#include "MainThread.h"

#define MY_QUIT (SDL_USEREVENT+1)

MainThread::MainThread(void)
{
	video_width_ = video_height_ = 0;
	start();
	media_ = 0;

    x_ = y_ = 0;
    width_ = height_ = 0;
}

MainThread::~MainThread(void)
{
	delete media_;

	req(CODE_QUIT);
	join();
}

void MainThread::run()
{
	int flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;

	if (SDL_Init(flags)) {
		fprintf(stderr, "ERR: SDL_Init fault!\n");
		return ;
	}

	event_loop();
}

void MainThread::event_loop()
{
	bool quit = false;
	while (!quit) {
		SDL_Event ev;
		SDL_PumpEvents();

		while (!SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
			int code;
			void *input;
			if (chk_req(10, &code, &input)) {
				do_req(code, input);
			}
			frame_refresh(now());
			SDL_PumpEvents();
		}

		switch (ev.type) {
		case SDL_MOUSEBUTTONDOWN:
			break;

		case SDL_MOUSEBUTTONUP:
			break;

		case MY_QUIT:
			quit = true;
			break;

		default:
			break;
		}
	}
}

void MainThread::do_req(int code, void *input)
{
	void *output = 0;

	switch (code) {
	case CODE_QUIT:
		{
			SDL_Event ev;
			ev.type = MY_QUIT;
			SDL_PushEvent(&ev);

			reply(CODE_QUIT);
		}
		break;

	case CODE_OPEN:
		code = open_with_wnd_internal((ParamsOpen*)input);
		break;

	case CODE_CLOSE:
		break;

	case CODE_VIDEO_CHANGED:
		code = video_changed_internal((AVFrame*)input);
		break;
	}
}

void MainThread::frame_refresh(double now)
{
	if (evt_fifo_.wait(10)) {
		evt_fifo_.reset();

		cs_fifo_.enter();
		if (fifo_.empty()) {
			cs_fifo_.leave();
            return;
		}

		CachedFrame *cf = fifo_.front();
		fifo_.pop_front();
		cs_fifo_.leave();

		unsigned char *d;
		int pitch;
		SDL_LockTexture(texture_, 0, (void**)&d, &pitch);

		unsigned char *s = cf->pic.data[0];
		for (int i = 0; i < cf->height; i++) {
			memcpy(d, s, cf->width);
			s += pitch;
			d += cf->pic.linesize[0];
		}

		s = cf->pic.data[2];
		for (int i = 0; i < cf->height/2; i++) {
			memcpy(d, s, cf->width/2);
			s += pitch / 2;
			d += cf->pic.linesize[2];
		}

		s = cf->pic.data[1];
		for (int i = 0; i < cf->height/2; i++) {
			memcpy(d, s, cf->width/2);
			s += pitch / 2;
			d += cf->pic.linesize[1];
		}

		SDL_UnlockTexture(texture_);

        SDL_Rect dst = { x_, y_, width_, height_ };
        SDL_RenderCopy(render_, texture_, 0, &dst);

		draw_others();

		SDL_RenderPresent(render_);

		cs_cached_.enter();
		cached_.push(cf);
		evt_cached_.signal();
		cs_cached_.leave();
	}
}

int MainThread::open_with_wnd_internal(MainThread::ParamsOpen *po)
{
	media_ = new MediaThread(po->url, this);

    reply(0);

	win_ = SDL_CreateWindowFrom(po->wnd);
	// win_ = SDL_CreateWindow("sdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 540, SDL_WINDOW_RESIZABLE);
    //render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
    render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_SOFTWARE);

	texture_ = 0;

	return 0;
}

void MainThread::close()
{
}

void MainThread::set_window_rect(int x, int y, int width, int height)
{
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
}

int MainThread::save_video_frame(const AVFrame *origin)
{
	if (video_width_ != origin->width || video_height_ != origin->height) {
		req(CODE_VIDEO_CHANGED, (void*)origin);
		video_width_ = origin->width;
		video_height_ = origin->height;
	}

	CachedFrame *cf = lock_video_cache(origin);
	if (cf) {
		cf->pts = origin->pts;

		unsigned char *s = origin->data[0], *d = cf->pic.data[0];
		for (int i = 0; i < origin->height; i++) {
			memcpy(d, s, origin->width);
			s += origin->linesize[0];
			d += cf->pic.linesize[0];
		}

		s = origin->data[1], d = cf->pic.data[1];
		for (int i = 0; i < origin->height/2; i++) {
			memcpy(d, s, origin->width/2);
			s += origin->linesize[1];
			d += cf->pic.linesize[1];
		}

		s = origin->data[2], d = cf->pic.data[2];
		for (int i = 0; i < origin->height/2; i++) {
			memcpy(d, s, origin->width/2);
			s += origin->linesize[2];
			d += cf->pic.linesize[2];
		}

		unlock_video_cache(cf);

		return 0;
	}
	else {
		return -1;
	}
}

int MainThread::video_changed_internal(AVFrame *f)
{
	if (texture_) {
		SDL_DestroyTexture(texture_);
	}

	texture_ = SDL_CreateTexture(render_, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, f->width, f->height);

	ost::MutexLock al(cs_cached_);

	for (int i = 0; i < 5; i++) {
		CachedFrame *cf = new CachedFrame;
		avpicture_alloc(&cf->pic, (AVPixelFormat)f->format, f->width, f->height);
		cf->width = f->width;
		cf->height = f->height;
		cached_.push(cf);
	}

	reply(0);

	return 0;
}

MainThread::CachedFrame *MainThread::lock_video_cache(const AVFrame *origin)
{
	cs_cached_.enter();
	while (cached_.empty()) {
		cs_cached_.leave();

		evt_cached_.wait();
		evt_cached_.reset();
		cs_cached_.enter();
	}

	CachedFrame *cf = cached_.top();
	cached_.pop();
	cs_cached_.leave();

	return cf;
}

void MainThread::unlock_video_cache(MainThread::CachedFrame *p)
{
	cs_fifo_.enter();
	fifo_.push_back(p);
	cs_fifo_.leave();

	evt_fifo_.signal();
}

void MainThread::draw_others()
{
	ost::MutexLock al(cs_points_);

	if (points_.empty()) {
		return;
	}

	SDL_SetRenderDrawColor(render_, 255, 255, 0, 255);

	SDL_Point p0 = points_.front();

	SDL_Rect r = { p0.x-3, p0.y-3, 6, 6 };
	SDL_RenderDrawRect(render_, &r);

	SDL_SetRenderDrawColor(render_, 255, 0, 0, 255);

	if (points_.size() > 1) {
		std::vector<SDL_Point>::const_iterator it = points_.begin();
		SDL_Point p1 = *it;

		for (++it; it != points_.end(); ++it) {
			SDL_Point p2 = *it;
			SDL_RenderDrawLine(render_, p1.x, p1.y, p2.x, p2.y);

			p1 = p2;
		}

		SDL_SetRenderDrawColor(render_, 0, 255, 255, 255);
		SDL_RenderDrawLine(render_, points_.back().x, points_.back().y, points_.front().x, points_.front().y);
	}
}

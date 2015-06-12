#include "zkrender.h"
#include "MainThread.h"
extern "C" {
#   include <libavcodec/avcodec.h>
#   include <libavformat/avformat.h>
}

class InitFFmpeg
{
public:
    InitFFmpeg()
    {
        av_register_all();
        avcodec_register_all();
        avformat_network_init();
    }
};

static InitFFmpeg __init_ffmpeg;

zkrender_t *zkr_open_with_wnd(const char *url, void *wnd)
{
    MainThread *t = new MainThread;
    t->open_with_wnd(url, wnd);
    return (zkrender_t *)t;
}

void zkr_close(zkrender_t *zkr)
{
    MainThread *t = (MainThread*)zkr;
    delete t;
}

void zkr_set_points(zkrender_t *zkr, const std::vector<Point> &points)
{
    MainThread *t = (MainThread*)zkr;

    std::vector<SDL_Point> ps;
    for (int i = 0; i < points.size(); i++) {
        SDL_Point pt = { points[i].x, points[i].y };
        ps.push_back(pt);
    }

    t->update_points(ps);
}

void zkr_set_window_rect(zkrender_t *zkr, int x, int y, int width, int height)
{
    MainThread *t = (MainThread *)zkr;

    t->set_window_rect(x, y, width, height);
}

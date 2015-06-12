#ifndef __zk_render__hh
#define __zk_render__hh

#include <vector>

typedef struct zkrender_t zkrender_t;

int zkr_init();
void zkr_uninit();

zkrender_t *zkr_open_with_wnd(const char *url, void *wnd);
void zkr_close(zkrender_t *zkr);
void zkr_set_window_rect(zkrender_t *zkr, int x, int y, int width, int height);

struct Point
{
	int x, y;
};
void zkr_set_points(zkrender_t *zkr, const std::vector<Point> &points);

#endif // zkrender.h

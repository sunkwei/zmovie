#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <sys/time.h>
#include <math.h>

static double _last_stamp[32];
static double _first = -1.0;

static double now()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static double uptime()
{
	return now() - _first;
}

static void print_stamp_delta()
{
	if (_last_stamp[0] > 0.0) {
		double delta = _last_stamp[0] - _last_stamp[1];
		static double max_delta = 0.0;
		if (max_delta < fabs(delta)) {
			max_delta = fabs(delta);
		}

		fprintf(stdout, "[%.1f]: s1: %.3f, s2: %.3f, delta: %.3f, max: %.3f\n",
				uptime(), _last_stamp[0], _last_stamp[1], 
				delta, max_delta);
	}
}

int main(int argc, char **argv)
{
	AVFormatContext *ic = 0;
	int rc;
	const char *url;
	size_t cnt = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <rtsp url>\n", argv[0]);
		return -1;
	}

	for (int i = 0; i < 32; i++)
		_last_stamp[i] = -1.0;

	url = argv[1];

	av_register_all();
	avformat_network_init();
	av_log_set_level(AV_LOG_FATAL);

	rc = avformat_open_input(&ic, url, 0, 0);
	if (rc < 0) {
		fprintf(stderr, "can't open %s\n", url);
		return -1;
	}

	rc = avformat_find_stream_info(ic, 0);
	if (rc < 0) {
		fprintf(stderr, "can't find %s stream info\n", url);
		return -1;
	}

	av_dump_format(ic, -1, 0, 0);

	_first = now();

	do {
		AVPacket pkg;
#if LIBAVFORMAT_VERSION_MAJOR >= 56
		pkg.buf = 0;
#endif //

		rc = av_read_frame(ic, &pkg);
		if (rc < 0) {
			break;
		}
		else {
			int idx = pkg.stream_index;
			AVStream *stream = ic->streams[idx];
			_last_stamp[idx] = pkg.pts * 1.0 * stream->time_base.num /
				stream->time_base.den;

//			fprintf(stderr, "pts=%lld, stamp=%.3f\n", pkg.pts, 
//					pkg.pts * 1.0 * stream->time_base.num / stream->time_base.den);
		}

#if LIBAVFORMAT_VERSION_MAJOR >= 56
		av_free_packet(&pkg);
#endif

		if (cnt++ % 100 == 0) {
			print_stamp_delta();
		}

	} while (1);

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>

static double _last_stamp[16] = { 0.0, 0.0, };

int main(int argc, char **argv)
{
	AVFormatContext *ic = 0;
	int rc;
	const char *url;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <rtsp url>\n", argv[0]);
		return -1;
	}

	url = argv[1];

	av_register_all();
	avformat_network_init();

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

	do {
		AVPacket pkg;
		pkg.buf = 0;

		rc = av_read_frame(ic, &pkg);
		if (rc < 0) {
			break;
		}
		else {
			int idx = pkg.stream_index;
			AVCodecContext *cc = ic->streams[idx]->codec;
			

		}
		

		av_free_packet(&pkg);
	} while (1);

	return 0;
}


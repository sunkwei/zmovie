#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
extern "C" {
#	include <libavcodec/avcodec.h>
#	include <libavformat/avformat.h>
}
#include <string>

struct Arguments
{
	std::string url;
	int width, height;
};

static int parse_cmdline(int argc, char **argv, Arguments *args)
{
	int rc = 0;

	int arg = 1;
	while (arg < argc) {
		if (argv[arg][0] == '-') {
			const char *p = &argv[arg][1];
			if (!strcmp(p, "width")) {
				if (arg+1 < argc) {
					args->width = atoi(p);
					arg++;
				}
				else {
					fprintf(stderr, "ERR: -width <WIDTH>\n");
					return -1;
				}
			}
			if (!strcmp(p, "height")) {
				if (arg+1 < argc) {
					args->height = atoi(p);
					arg++;
				}
				else {
					fprintf(stderr, "ERR: -height <HEIGHT>\n");
					return -1;
				}
			}
		}
		else {
			args->url = argv[arg];
		}

		arg++;
	}

	return rc;
}

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERR: unable init sdl video sys\n");
		return -1;
	}

	av_register_all();
	avcodec_register_all();
	avformat_network_init();

	Arguments arg;
	arg.width = 960;
	arg.height = 540;
	
	if (parse_cmdline(argc, argv, &arg) < 0) {
		fprintf(stderr, "ERR: cmdline parse err\n");
		return -1;
	}

	if (arg.url.empty()) {
		fprintf(stderr, "ERR: MUST supply url\n");
		return -1;
	}

	AVFormatContext *avctx = 0;
	int rc = avformat_open_input(&avctx, arg.url.c_str(), 0, 0);
	if (rc < 0) {
		fprintf(stderr, "ERR: can't open url %s\n", arg.url.c_str());
		return -1;
	}

	rc = avformat_find_stream_info(avctx, 0);
	if (rc < 0) {
		fprintf(stderr, "ERR: can't find stream info of %s\n", arg.url.c_str());
		return -1;
	}

	av_dump_format(avctx, 0, arg.url.c_str(), 0);

	for (int i = 0; i < avctx->nb_streams; i++) {
		AVCodecContext *ctx = avctx->streams[i]->codec;
		AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
		if (!codec) {
			fprintf(stderr, "ERR: NO decoder for %d\n", ctx->codec_id);
			return -1;
		}
		else {
			fprintf(stderr, "DEBUG: find decoder for %d\n", ctx->codec_id);
		}

		avcodec_open2(ctx, codec, 0);
	}

	

	return 0;
}


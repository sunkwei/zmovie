#include "MediaThread.h"
#include "DecodeResult.h"

class FFmpegInit
{
public:
    FFmpegInit()
    {
        av_register_all();
        avcodec_register_all();
    }
};

static FFmpegInit _ffmpeg_init;

MediaThread::MediaThread(const char *url, DecodeResult *dr)
	: url_(url)
	, dr_(dr)
{
	start();
}

MediaThread::~MediaThread(void)
{
	req(0);
	join();
}

int MediaThread::decode_frame(AVFrame *frame, AVPacket *pkg)
{
	int got = 0, rc;

	if (pkg->size == 0) {
		return 0;
	}

	AVCodecContext *decoder = ic_->streams[pkg->stream_index]->codec;
	if (decoder->codec_type == AVMEDIA_TYPE_VIDEO) {
		rc = avcodec_decode_video2(decoder, frame, &got, pkg);
		if (got) {
			frame->pts = pkg->pts;

			pkg->data += rc;
			pkg->size -= rc;
		}
	}

	return got;
}

void MediaThread::run()
{
	const char *url = url_.c_str();
	ic_ = 0;

	int rc = avformat_open_input(&ic_, url, 0, 0);
	if (rc < 0) {
		fprintf(stderr, "ERR: avformat_open_input can't open %s\n", url);
		return;
	}

	rc = avformat_find_stream_info(ic_, 0);
	if (rc < 0) {
		fprintf(stderr, "ERR: avformat_find_stream_info fault!\n");
		avformat_close_input(&ic_);
		return;
	}

	av_dump_format(ic_, -1, url, 0);

	for (int i = 0; i < ic_->nb_streams; i++) {
		AVCodecContext *ctx = ic_->streams[i]->codec;
		AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
		if (!codec) {
			fprintf(stderr, "ERR: avcodec_find_decoder NOT find %d\n", ctx->codec_id);
			avformat_close_input(&ic_);
			return;
		}

		avcodec_open2(ctx, codec, 0);
	}

	bool err = false;
	AVFrame *frame = av_frame_alloc();

	while (1) {
		int code;
		if (chk_req(0, &code)) {
			if (code == 0) {
				reply(0);
                break;
			}
		}

		if (err) {
			code = get_req();
			if (code == 0) {
				reply(0);
				break;
			}
		}
		else {
			AVPacket pkg;
			rc = av_read_frame(ic_, &pkg);
			if (rc < 0) {
				err = true;
				continue;
			}

			int got;
			do {
				got = decode_frame(frame, &pkg);
				if (got) {
					dr_->save_video_frame(frame);
				}
			} while (got);

			av_free_packet(&pkg);
		}
	}

	avformat_close_input(&ic_);
	av_frame_free(&frame);
}

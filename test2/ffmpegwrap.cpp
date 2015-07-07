#include "ffmpegwrap.h"
#include <QtGlobal>

class FFMPEGInit
{
public:
    FFMPEGInit()
    {
        av_register_all();
        avcodec_register_all();
        avformat_network_init();
        av_log_set_callback(log);
    }

    static void log(void *avcl, int level, const char *fmt, va_list args)
    {
        char line[1024];
        int print_prefix = 1;
        av_log_format_line(avcl, level, fmt, args, line, sizeof(line), &print_prefix);
        qDebug(line);
    }
};

static FFMPEGInit __ffmpeg_init;

ffmpegWrap::ffmpegWrap(const char *url, ffmpegDecoderCallback *cb)
    : url_(url)
    , cb_(cb)
{
    ic_ = 0;
    sws_ = 0;
    frame_ = av_frame_alloc();
    pkg_.data = 0;
    pkg_.size = 0;
    pkg_.buf = 0;
    audio_sample_cnt_ = 0;
}

ffmpegWrap::~ffmpegWrap()
{
    av_frame_free(&frame_);

    if (sws_) {
        sws_freeContext(sws_);
    }
    if (ic_) {
        avformat_close_input(&ic_);
    }
}

int ffmpegWrap::run_once()
{
    if (!ic_) {
        if (open_url() < 0) {
            return RC_FAILURE;
        }
    }

    return next_frame();
}

int ffmpegWrap::open_url()
{
    AVDictionary *dict = 0;
    av_dict_set(&dict, "stimeout", "2000000", 0);	// stimeout 为 tcp io 超时，微秒
    int rc = avformat_open_input(&ic_, url_.c_str(), 0, &dict);
    av_dict_free(&dict);
    if (rc != 0) {
        return rc;
    }

    rc = avformat_find_stream_info(ic_, 0);
    if (rc < 0) {
        avformat_close_input(&ic_);
        return rc;
    }

    for (int i = 0; i < ic_->nb_streams; i++) {
        AVCodecContext *cc = ic_->streams[i]->codec;
        AVCodec *codec = avcodec_find_decoder(cc->codec_id);
        if (codec) {
            avcodec_open2(cc, codec, 0);
        }
    }

    return 0;
}

int ffmpegWrap::next_frame()
{
    int rc = RC_FAILURE;

    if (pkg_.size == 0) {
        rc = av_read_frame(ic_, &pkg_);
        if (rc < 0) {
            return RC_FAILURE;
        }
    }

    rc = decode_frame(ic_->streams[pkg_.stream_index], &pkg_);

    if (pkg_.size == 0) {
        av_free_packet(&pkg_);
    }

    return rc;
}

int ffmpegWrap::decode_frame(AVStream *stream, AVPacket *pkt)
{
    int rc = RC_FAILURE;
    int got;
    if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
        rc = avcodec_decode_audio4(stream->codec, frame_, &got, pkt);
        if (got) {
            double stamp = audio_sample_cnt_ * 1.0 / frame_->sample_rate;
            cb_->on_audio_frame(pkt->stream_index, frame_, stamp);
            audio_sample_cnt_ += frame_->nb_samples;
        }

        if (rc > 0) {
            pkt->data += rc;
            pkt->size -= rc;
        }

        if (rc < 0) {
            return rc;
        }

        return 0;
    }
    else if (stream->codec->codec_type == AVMEDIA_TYPE_VIDEO){
        rc = avcodec_decode_video2(stream->codec, frame_, &got, pkt);
        if (got) {
            double stamp = pkt->pts * 1.0 * stream->time_base.num / stream->time_base.den;
            cb_->on_video_frame(pkt->stream_index, frame_, stamp);
        }

        if (rc > 0) {
            pkt->data += rc;
            pkt->size -= rc;
        }

        if (rc < 0) {
            return rc;
        }

        return 0;
    }
    else {
        pkt->size = 0;  // :)
        return 0;
    }
}

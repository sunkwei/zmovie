#include "ffmpegwrap.h"

class FFMPEGInit
{
public:
    FFMPEGInit()
    {
        av_register_all();
        avcodec_register_all();
        avformat_network_init();
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
    int rc = avformat_open_input(&ic_, url_.c_str(), 0, 0);
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

    rc = decode_frame(ic_->streams[pkg_.stream_index]->codec, &pkg_);

    if (pkg_.size == 0) {
        av_free_packet(&pkg_);
    }

    return rc;
}

int ffmpegWrap::decode_frame(AVCodecContext *cc, AVPacket *pkt)
{
    int rc = RC_FAILURE;
    int got;
    if (cc->codec_type == AVMEDIA_TYPE_AUDIO) {
        rc = avcodec_decode_audio4(cc, frame_, &got, pkt);
        if (got) {
            cb_->on_audio_frame(pkt->stream_index, frame_, 0.0);
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
    else if (cc->codec_type == AVMEDIA_TYPE_VIDEO){
        rc = avcodec_decode_video2(cc, frame_, &got, pkt);
        if (got) {
            double stamp = pkt->pts * 1.0 * cc->time_base.num / cc->time_base.den;
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

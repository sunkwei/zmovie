#include "mediasource.h"
#include "log.h"

MediaSource::MediaSource(const char *fname)
{
    quit_ = true;
    fname_ = fname;
    ctx_ = avformat_alloc_context();
    decode_audio_ = avcodec_decode_audio4;
    decode_video_ = avcodec_decode_video2;
}

MediaSource::~MediaSource()
{
    avformat_free_context(ctx_);
}

int MediaSource::open()
{
    int ret = avformat_open_input(&ctx_, fname_.c_str(), 0, 0);
    if (ret == 0) {
        Log::log(INFO, "open media file '%s' OK\n", fname_.c_str());

        if (avformat_find_stream_info(ctx_, 0) < 0) {
            Log::log(ERROR, "  can't find media streams info!\n");
            avformat_close_input(&ctx_);
            return -2;
        }

        for (uint32_t i = 0; i < ctx_->nb_streams; i++) {
            AVCodecContext *codec = ctx_->streams[i]->codec;
            AVCodec *c = avcodec_find_decoder(codec->codec_id);
            if (!c) {
                Log::log(ERROR, "  can't find matched decoder for sid:%d codec id of %d\n", i, codec->codec_id);
                return -3;
            }
            else {
                avcodec_open2(codec, c, 0);
                Log::log(INFO, "  sid:%d opened\n", i);
            }
        }

        quit_ = false;
        playing_ = false;
        QThread::start();

        return 0;
    }
    else {
        Log::log(ERROR, "open media file '%s' fault!\n", fname_.c_str());
        return -1;
    }
}

void MediaSource::close()
{
    if (!quit_) {
        quit_ = true;
        QThread::wait();

        avformat_close_input(&ctx_);
    }
}

int MediaSource::play()
{
    playing_ = true;
    return -1;
}

int MediaSource::pause()
{
    playing_ = false;
    return 1;
}

void MediaSource::run()
{
    /** 解码，并且调用 one_frame()
     */
    while (!quit_) {
        if (!playing_) {
            QThread::msleep(40);
            continue;
        }

        AVPacket pkg;
        int ret = av_read_frame(ctx_, &pkg);
        if (ret < 0) {
            Log::log(WARNING, "'%s' read err!\n", fname_.c_str());
            QThread::msleep(40);
            continue;
        }
        else {
            decode_one_frame(pkg);
        }
    }
}

void MediaSource::decode_one_frame(AVPacket &pkg)
{
    AVCodecContext *decoder = ctx_->streams[pkg.stream_index]->codec;
    pfn_decode decode = 0;
    if (decoder->codec_type == AVMEDIA_TYPE_VIDEO) {
        decode = decode_video_;
    }
    else if (decoder->codec_type == AVMEDIA_TYPE_AUDIO) {
        decode = decode_audio_;
    }

    if (decode) {
        uint8_t *ptr = pkg.data;
        int rest = pkg.size;

        while (rest > 0) {
            AVFrame frame;
            int got;
            pkg.data = ptr;
            pkg.size = rest;

            int bytes = decode(decoder, &frame, &got, &pkg);
            if (bytes < 0) {
                Log::log(WARNING, "'%s' decode track error!!\n", fname_.c_str());
            }
            else {
                ptr += bytes;
                rest -= bytes;

                if (got) {
                    emit one_frame(decoder->codec_type == AVMEDIA_TYPE_AUDIO,
                                   pkg.stream_index, &frame);
                    av_frame_unref(&frame);
                }
            }
        }
    }
}

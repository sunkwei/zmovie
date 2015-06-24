#include "ImageSource.h"
#include <opencv2/opencv.hpp>

extern "C" {
#	include <libavcodec/avcodec.h>
}

ImageSource::ImageSource(void)
	: quit_(true)
{
	cfg_ = 0;
	win_x_ = -1, win_y_ = -1;
	enable_movie_ = false;
	win_movie_ = 0;
}

ImageSource::~ImageSource(void)
{
}

void ImageSource::start(KVConfig *cfg)
{
	cfg_ = cfg;
	quit_ = false;
	paused_ = false;
	paused_want_ = false;
	ost::Thread::start();
}

void ImageSource::stop()
{
	if (!quit_) {
		quit_ = true;
		ost::Thread::join();
	}
}

void ImageSource::pause()
{
	paused_want_ = true;

	ost::MutexLock al(cs_detect_result_);
	DetectResult dr;
	detect_result_ = dr;
}

void ImageSource::resume()
{
	paused_want_ = false;
}

KVConfig *ImageSource::cfg()
{
	return cfg_;
}

void ImageSource::set_win_name(const char *name, int x, int y)
{
	if (name) {
		win_name_ = name;
		win_x_ = x, win_y_ = y;
	}
}

static IplImage *zifImage2IplImage (zifImage *img)
{
	IplImage *image = (IplImage*)malloc(sizeof(IplImage));
	image->nSize = sizeof(IplImage);
	image->ID = 0;
	image->nChannels = 3;
	image->alphaChannel = 0;
	image->depth = 8;
	image->origin = 0;
	image->dataOrder = 0;
	image->width = img->width;
	image->height = img->height;

	image->roi = 0;
	image->imageId = 0;
	image->tileInfo = 0;

	AVPicture *pic = (AVPicture *)img->internal_ptr;

	image->imageSize = pic->linesize[0] * img->height;
	image->widthStep = pic->linesize[0];
	image->imageData = (char*)pic->data[0];

	image->imageDataOrigin = 0;

	return image;
}

void ImageSource::run()
{
	const char *url = is_detecting() 
		? cfg_->get_value("video_source", "tcp://localhost:3000") 
		: cfg_->get_value("video_source_tracing", "tcp://localhost:3000");

	imgsrc_format fmt;
	fmt.fmt = PIX_FMT_BGR24;
	fmt.fps = 1.0; // 
	fmt.width = atoi(cfg_->get_value("video_width", "960"));
	fmt.height = atoi(cfg_->get_value("video_height", "540"));

	util_log(LOG_DEBUG, "DEBUG: ��������Դ��url='%s', fmt=%d-%d\n", url, fmt.width, fmt.height);

	imgsrc_t *src = imgsrc_open(url, &fmt);
	if (!src) {
		util_log(LOG_FAULT, "FAULT: �޷���������Դ�� url='%s'���������̣�����\n", url);
		::exit(-1);
	}

	if (!win_name_.empty()) {
		cvNamedWindow(win_name_.c_str());

		if (win_x_ >= 0 && win_y_ >= 0)
			cvMoveWindow(win_name_.c_str(), win_x_, win_y_);
	}

	int en = 0;	// ���� imgsrc_next() ʧ�ܴ��� ..

	while (!quit_) {
		zifImage *img = imgsrc_next(src);
		if (!img) {
			sleep(50);
			en++;

			if (en > 50) {
				util_log(LOG_WARNING, "WARNING: ��ȡͼ��ʧ�ܣ� url='%s'\n", url);
				en = 0;
			}

			continue;
		}
		else {
			en = 0;
		}

		if (paused_want_ != paused_) {
			if (paused_want_) {
				before_pause();
			}
			else {
				before_resume();
			}

			paused_ = paused_want_;
		}

		/** ����ͣ�󣬲�����:
				1. ̽�⣨�ķ�cpu)
				2. ������̨����̨�������������Ѿ��ж�) ...
				3. �л������ܽ����ֶ�����״̬�����ܷ��л������� ) ..
		 */
		if (!paused_) {
			one_frame(img);
		}

		if (_debug == 1 || (_debug == 2 && is_detecting())) {
			if (!win_name_.empty()) {
				IplImage *pic = zifImage2IplImage(img);
				cvShowImage(win_name_.c_str(), pic);	// ���� opencv ��ʾ ..

				if (enable_movie_ && win_movie_) {
					win_movie_->show(pic);
				}

				free(pic);
			}
			cvWaitKey(1);
		}

		imgsrc_free(src, img);
	}

	util_log(LOG_DEBUG, "DEBUG: �ر�����Դ��url='%s'\n", url);
	imgsrc_close(src);
}

DetectResult ImageSource::get_last_detect_result()
{
	ost::MutexLock al(cs_detect_result_);
	return detect_result_;
}

void ImageSource::set_detect_result(const DetectResult &dr)
{
	ost::MutexLock al(cs_detect_result_);

	int st = dr.rcs.empty() ? 0 : dr.rcs.size() == 1 ? 1 : 2;

	if (cs_.set_state(st)) {
		detect_result_ = dr;
	}
}

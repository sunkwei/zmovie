#pragma once

#include <cc++/thread.h>
#include "../../image_trace/libkvconfig/KVConfig.h"
#include "../../image_trace/libimagesource/image_source.h"
#include "util.h"
#include "StateDuration.h"
#include "DebugMovieWin.h"

class ImageSource : ost::Thread
{
protected:
	bool quit_;
	KVConfig *cfg_;
	std::string win_name_;
	
	bool enable_movie_;
	DebugMovieWin *win_movie_;

	int win_x_, win_y_;	// ��ʾλ�� 
	bool paused_, paused_want_;
	DetectResult detect_result_;	// ��������ʵ�֣����� ...
	ost::Mutex cs_detect_result_;
	ContinousState cs_;	// ��Ҫ���� 5 ����Ч ..

public:
	ImageSource(void);
	virtual ~ImageSource(void);

	void start(KVConfig *cfg);
	void stop();

	void pause();	// ��ͣ ...
	void resume();	// �ָ� ...

	/** �������µ�̽���� ... */
	DetectResult get_last_detect_result();

	// ������ʾ�������� ...
	void set_win_name(const char *name, int x = -1, int y = -1);

	// ������ʾ���ĸ��Ӵ��ڵ����֣�һ��������ڼ����л�Ч��;
	void set_movie_win(DebugMovieWin *win) { win_movie_ = win; }
	void enable_movie_output(bool enable) { enable_movie_ = enable; }

protected:
	virtual void run();

protected:
	// �����Ƿ�̽��Դ ...
	virtual bool is_detecting()
	{
		return true;
	}

	// �������д���һ֡ͼ�� ...
	virtual void one_frame(zifImage *image) = 0;

	/** ̽��ʵ�֣�����̽���� */
	void set_detect_result(const DetectResult &dr);

	virtual void before_pause() {}
	virtual void before_resume() {}

protected:
	KVConfig *cfg();
};

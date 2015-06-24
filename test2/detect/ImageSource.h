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

	int win_x_, win_y_;	// 显示位置 
	bool paused_, paused_want_;
	DetectResult detect_result_;	// 派生类中实现，保存 ...
	ost::Mutex cs_detect_result_;
	ContinousState cs_;	// 需要连续 5 次有效 ..

public:
	ImageSource(void);
	virtual ~ImageSource(void);

	void start(KVConfig *cfg);
	void stop();

	void pause();	// 暂停 ...
	void resume();	// 恢复 ...

	/** 返回最新的探测结果 ... */
	DetectResult get_last_detect_result();

	// 设置显示窗口名字 ...
	void set_win_name(const char *name, int x = -1, int y = -1);

	// 设置显示到的附加窗口的名字，一般仅仅用于监视切换效果;
	void set_movie_win(DebugMovieWin *win) { win_movie_ = win; }
	void enable_movie_output(bool enable) { enable_movie_ = enable; }

protected:
	virtual void run();

protected:
	// 返回是否探测源 ...
	virtual bool is_detecting()
	{
		return true;
	}

	// 派生类中处理一帧图像 ...
	virtual void one_frame(zifImage *image) = 0;

	/** 探测实现，保存探测结果 */
	void set_detect_result(const DetectResult &dr);

	virtual void before_pause() {}
	virtual void before_resume() {}

protected:
	KVConfig *cfg();
};

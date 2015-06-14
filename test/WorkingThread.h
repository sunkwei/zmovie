#pragma once

#include <cc++/thread.h>

class Thread : ost::Thread
{
	ost::Event req_notify_, result_notify_;
	int code_, result_;
	ost::Mutex lock_;
	void *input_, *output_;

public:
	Thread()
	{
	}

	~Thread()
	{
	}

	static double now()
	{
		timeval tv;
#ifdef WIN32
		ost::gettimeofday(&tv, 0);
#else
        gettimeofday(&tv, 0);
#endif
		return tv.tv_sec + tv.tv_usec / 1000000.0;
	}

	void start()
	{
		ost::Thread::start();
	}

	void join()
	{
		ost::Thread::join();
	}

public:
	int req(int code, void *input = 0, void **output = 0)
	{
		ost::MutexLock al(lock_);

		code_ = code;
		input_ = input;
		req_notify_.signal();

		result_notify_.wait();
		result_notify_.reset();

		if (output) {
			*output = output_;
		}

		return result_;
	}

protected:
	void reply(int result, void *output = 0)
	{
		result_ = result;
		output_ = output;
		result_notify_.signal();
	}

	bool chk_req(int timeout, int *code, void **input = 0)
	{
		if (req_notify_.wait(timeout)) {
			*code = code_;
			if (input) {
				*input = input_;
			}
			req_notify_.reset();
			return true;
		}
		else {
			return false;
		}
	}

	int get_req(void **input = 0)
	{
		req_notify_.wait();
		req_notify_.reset();
		if (input) {
			*input = input_;
		}
		return code_;
	}

private:
};

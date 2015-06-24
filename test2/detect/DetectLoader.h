#pragma once

#include "../zifimage.h"
#ifdef WIN32
#   include <Windows.h>
#else
#   include <dlfcn.h>
    typedef void *HMODULE;
#endif
#include <string>

class DetectLoader
{
	HMODULE dll_;

	typedef void *(*PFN_open)(const char*);
	typedef void (*PFN_close)(void*);
	typedef char*(*PFN_det)(void *, zifImage*);
	typedef void (*PFN_clr)(void*);
	typedef void (*PFN_set_param)(void*, int, int, double, double, double);
	typedef char*(*PFN_det_4l)(void *, zifImage*, zifImage*, zifImage*, zifImage*);
	typedef void (*PFN_enable_gpu)(void *, int);
	typedef void (*PFN_set_flipped_mode)(void *, int);

	PFN_open func_open_;
	PFN_close func_close_;
	PFN_enable_gpu func_gpu_;
	PFN_det func_det_;
    PFN_clr func_clear_;
	PFN_set_param func_set_param_;
	PFN_det_4l func_det_4l_;
	PFN_set_flipped_mode func_set_flipped_mode_;

	void *det_;
	std::string cfg_fname_, dll_fname_;

public:
	DetectLoader(const char *dll_fname, const char *cfg_fname);
	~DetectLoader(void);

	const char *detect(zifImage *img);
	const char *detect(zifImage *s0, zifImage *s1, zifImage *s2, zifImage *s3);
	void reopen();
	void close();

	void clr();
	void set_param(int p0, int p360, double t0, double t360, double dis);
	void set_flipped_mode(int e);
};

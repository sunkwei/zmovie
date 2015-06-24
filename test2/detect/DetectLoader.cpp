#include "DetectLoader.h"

#ifdef WIN32
#else
#   define LoadLibrary(fname) dlopen(fname, RTLD_LAZY)
#   define GetProcAddress(mod, name) dlsym(mod, name)
#   define CloseHandle(mod) dlcloes(mod)
#endif

DetectLoader::DetectLoader(const char *dll_fname, const char *cfg_fname)
{
	func_clear_ = 0;
	cfg_fname_ = cfg_fname;
	dll_fname_ = dll_fname;

    SetDllDirectory("./runtime/bin/");

	dll_ = LoadLibrary(dll_fname);
	if (dll_) {
		func_open_ = (PFN_open)GetProcAddress(dll_, "det_open");
		func_close_ = (PFN_close)GetProcAddress(dll_, "det_close");
		func_det_ = (PFN_det)GetProcAddress(dll_, "det_detect");
		func_clear_ = (PFN_clr)GetProcAddress(dll_, "det_stop");
		func_set_param_ = (PFN_set_param)GetProcAddress(dll_, "det_set_param");
		func_det_4l_ = (PFN_det_4l)GetProcAddress(dll_, "det_detect_4l");
		func_gpu_ = (PFN_enable_gpu)GetProcAddress(dll_, "det_enable_gpu");
		func_set_flipped_mode_ = (PFN_set_flipped_mode)GetProcAddress(dll_, "det_set_flipped_mode");

		det_ = func_open_(cfg_fname);

		if (!det_) {
			::exit(-1);
		}

		if (func_gpu_) {
			func_gpu_(det_, 0);
		}
	}
	else {
		func_open_ = 0;
		func_clear_ = 0;
		func_det_ = 0;
		func_clear_ = 0;
		func_set_param_ = 0;
		func_det_4l_ = 0;
		func_gpu_ = 0;
		func_set_flipped_mode_ = 0;

		det_ = 0;
		::exit(-1);
	}
}

DetectLoader::~DetectLoader(void)
{
	if (det_ && func_close_)
		func_close_(det_);

	if (dll_)
		FreeLibrary(dll_);
}

const char *DetectLoader::detect(zifImage *img)
{
	if (det_ && func_det_)
		return func_det_(det_, img);
	else
		return 0;
}

const char *DetectLoader::detect(zifImage *s0, zifImage *s1, zifImage *s2, zifImage *s3)
{
	if (det_ && func_det_4l_)
		return func_det_4l_(det_, s0, s1, s2, s3);
	else
		return 0;
}

void DetectLoader::reopen()
{
	if (func_open_) {
		if (det_ && func_close_) func_close_(det_);
		det_ = func_open_(cfg_fname_.c_str());
	}

	if (!det_) {
		::exit(-1);
	}
}

void DetectLoader::close()
{
	if (func_close_ && det_) {
		func_close_(det_);
	}

	det_ = 0;
}

void DetectLoader::clr()
{
	if (det_ && func_clear_)
		func_clear_(det_);
}

void DetectLoader::set_param(int p0, int p360, double t0, double t360, double dis)
{
	if (det_ && func_set_param_)
		func_set_param_(det_, p0, p360, t0, t360, dis);
}

void DetectLoader::set_flipped_mode(int e)
{
	if (det_ && func_set_flipped_mode_) {
		func_set_flipped_mode_(det_, e);
	}
}

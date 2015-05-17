#include <stdio.h>
#include <stdlib.h>
#include "common.h"

bool load_sample(const char *fname, int &label, cv::Mat &m)
{
	const char *_sdir[] = { "right", "down", "left", "up", };
	cv::Mat sample;

	fprintf(stdout, "INFO: try load from %s\n", fname);

	label = -1;

	cv::FileStorage fs(fname, cv::FileStorage::READ);
	std::string sdir;

	fs["dir"] >> sdir;
	fs["eigens"] >> sample;

	for (int i = 0; i < 4; i++) {
		if (sdir == _sdir[i]) {
			label = i;
			break;
		}
	}

	fs.release();

	// sample 为 unsigned char，归一化为 [0.0 - 1.0] 的 float 类型
	sample.convertTo(sample, CV_32FC1);
	cv::normalize(sample, m, 1.0, 0.0, cv::NORM_MINMAX);

	return label >= 0;

}


#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <cc++/file.h>
#include "common.h"


/** 读取指定目录的 yaml 文件，进行训练
 */
int main(int argc, char **argv)
{
	std::vector<int> labels;
	std::vector<cv::Mat> samples;
	char filename[256];

	ost::Dir dir("detlog/");
	while (dir.isValid()) {
		const char *fname = dir++;
		if (!fname) {
			break;
		}

		if (!strcmp(ost::File::getExtension(fname), ".yaml")) {
			int label;
			cv::Mat m;

			snprintf(filename, sizeof(filename), "detlog/%s", fname);
			if (load_sample(filename, label, m)) {
				labels.push_back(label);
				samples.push_back(m.reshape(1, 1));
			}
		}
	}

	cv::Mat mlables = cv::Mat(labels.size(), 1, CV_32FC1);
	for (int i = 0; i < mlables.rows; i++) {
		mlables.at<float>(i, 0) = labels[i];
	}

	cv::Mat msamples = cv::Mat(samples.size(), 64, CV_32FC1);
	for (int i = 0; i < msamples.rows; i++) {
		float *ptr = msamples.ptr<float>(i);
		float *ptr_s = samples[i].ptr<float>(0);

		for (int j = 0; j < 64; j++) {
			*ptr++ = *ptr_s++;
		}
	}

	CvSVM svm;
	svm.train(msamples, mlables);
	svm.save("detlog/result.xml");

	return 0;
}

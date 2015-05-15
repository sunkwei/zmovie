#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>

const char *_sdir[] = { "right", "down", "left", "up", };

static bool load_sample(const char *fname, int &label, cv::Mat &sample)
{
	fprintf(stdout, "INFO: load %s\n", fname);

	label = -1;

	cv::FileStorage fs(fname, cv::FileStorage::READ);
	std::string sdir;
	cv::Mat m;

	fs["dir"] >> sdir;
	fs["eigens"] >> m;

	for (int i = 0; i < 4; i++) {
		if (sdir == _sdir[i]) {
			label = i;
			break;
		}
	}

	fs.release();

	sample = cv::Mat(64, 1, CV_32FC1);
	m = m.reshape(1, 1);
	for (int i = 0; i < m.rows; i++) {
		sample.at<float>(i, 0) = m.at<unsigned char>(i, 0);
	}

	return label >= 0;
}

int main(int argc, char **argv)
{
	const char *fname = "detlog/e00000030_16-17--3.yaml";
	fname = "detlog/e00000162_16-17--1.yaml";
	fname = "detlog/e00000064_21-27--2.yaml";
	fname = "detlog/e00000031_15-16--3.yaml";
	fname = "detlog/e00000107_12-24--3.yaml";
	fname = "detlog/e00000106_13-25--3.yaml";

	int label;
	cv::Mat sample;
	if (!load_sample(fname, label, sample)) {
		fprintf(stderr, "ERR: can't load sample from %s\n", fname);
		return -1;
	}

	CvSVM svm;
	svm.load("detlog/result.xml");

	float idx = svm.predict(sample);
	fprintf(stderr, "INFO: get index=%.1f\n", idx);

	return 0;
}


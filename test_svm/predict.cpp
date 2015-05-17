#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include "common.h"

int main(int argc, char **argv)
{
	const char *fname = "detlog/e00000030_16-17--3.yaml";
	fname = "detlog/e00000064_21-27--2.yaml";
	fname = "detlog/e00000107_12-24--3.yaml";
	fname = "detlog/e00000106_13-25--3.yaml";
	fname = "detlog/e00000031_15-16--3.yaml";
	fname = "detlog/e00000162_16-17--1.yaml";

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


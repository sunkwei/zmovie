#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <cc++/file.h>
#include "common.h"

int main(int argc, char **argv)
{
	ost::Dir dir("verify");

	CvSVM svm;
	svm.load("detlog/result.xml");

	size_t total = 0, ok = 0;

	while (dir.isValid()) {
		const char *fname = dir++;
		if (!fname) {
			break;
		}

		if (!strcmp(ost::File::getExtension(fname), ".yaml")) {
			int label;
			cv::Mat sample;
			char filename[256];

			snprintf(filename, sizeof(filename), "verify/%s", fname);
			if (load_sample(filename, label, sample)) {
				total++;

				int idx = (int)svm.predict(sample);
				if (idx == label) {
					fprintf(stderr, "O");
					ok++;
				}
				else {
					fprintf(stderr, "E");
				}
			}
		}
	}

	fprintf(stderr, "test %u samples, and %u succeful!\n", total, ok);
	
	return 0;
}


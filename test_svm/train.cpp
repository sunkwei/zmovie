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
	std::vector < int >labels;
	std::vector < cv::Mat > samples;
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

			snprintf(filename, sizeof(filename), "detlog/%s",
				 fname);
			if (load_sample(filename, label, m)) {
				labels.push_back(label);
				samples.push_back(m.reshape(1, 1));
			}
		}
	}

	cv::Mat mlables = cv::Mat(labels.size(), 1, CV_32FC1);
	for (int i = 0; i < mlables.rows; i++) {
		mlables.at < float >(i, 0) = labels[i];
	}

	cv::Mat msamples = cv::Mat(samples.size(), 64, CV_32FC1);
	for (int i = 0; i < msamples.rows; i++) {
		float *ptr = msamples.ptr < float >(i);
		float *ptr_s = samples[i].ptr < float >(0);

		for (int j = 0; j < 64; j++) {
			*ptr++ = *ptr_s++;
		}
	}

#if 1
	CvParamGrid CvParamGrid_C(pow(2.0, -5), pow(2.0, 5), 1.1);
	CvParamGrid CvParamGrid_gamma(pow(2.0, -5), pow(2.0, 5), 1.1);
	if (!CvParamGrid_C.check() || !CvParamGrid_gamma.check())
		std::cout << "The grid is NOT VALID." << std::endl;

	CvSVMParams paramz;
	paramz.kernel_type = CvSVM::RBF;
	paramz.svm_type = CvSVM::C_SVC;
	paramz.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.000001);

	CvSVM svm;
	svm.train_auto(msamples, mlables, cv::Mat(), cv::Mat(), paramz, 10,
		CvParamGrid_C, CvParamGrid_gamma, CvSVM::get_default_grid(CvSVM::P), 
		CvSVM::get_default_grid(CvSVM::NU), CvSVM::get_default_grid(CvSVM::COEF),
		CvSVM::get_default_grid(CvSVM::DEGREE), true);

	paramz = svm.get_params();
	fprintf(stderr, "result: gamma=%f, C=%f, p=%f\n", paramz.gamma, paramz.C, paramz.p);
#else
	CvSVMParams paramz;
	paramz.svm_type = CvSVM::C_SVC;
	paramz.kernel_type = CvSVM::RBF;
	paramz.gamma = 20;
	paramz.C = 7;
	paramz.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.000001);

	CvSVM svm;
	svm.train(msamples, mlables, cv::Mat(), cv::Mat(), paramz);

#endif //

	svm.save("detlog/result.xml");

	return 0;
}

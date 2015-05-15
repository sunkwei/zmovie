#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <cc++/file.h>

const char *_sdir[] = { "right", "down", "left", "up", };

static bool load_sample(const char *fname, int &label, cv::Mat &m)
{
	fprintf(stdout, "INFO: load %s\n", fname);

	label = -1;

	cv::FileStorage fs(fname, cv::FileStorage::READ);
	std::string sdir;

	fs["dir"] >> sdir;
	fs["eigens"] >> m;

	for (int i = 0; i < 4; i++) {
		if (sdir == _sdir[i]) {
			label = i;
			break;
		}
	}

	fs.release();
	return label >= 0;
}

/** 读取指定目录的 yaml 文件，进行训练
 */
int main(int argc, char **argv)
{
	std::vector<int> labels;
	std::vector<std::vector<unsigned char> > samples;
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
				std::vector<unsigned char> v;
				for (int i = 0; i < m.cols; i++) {
					v.push_back(m.at<unsigned char>(0, i));
				}
				samples.push_back(v);
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
		std::vector<unsigned char> s = samples[i];
		for (int j = 0; j < samples.size(); j++) {
			ptr[j] = s[j];
		}
	}

	CvSVM svm;
	svm.train(msamples, mlables);
	svm.save("detlog/result.xml");

	return 0;
}

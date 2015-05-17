#pragma once

#include <opencv2/opencv.hpp>

/** 从 fname 中加载样本，并且归一化 */
bool load_sample(const char *fname, int &label, cv::Mat &sample);


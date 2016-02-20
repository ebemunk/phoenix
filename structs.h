#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

#include <opencv2/core/core.hpp>

struct qtable {
	int index;
	int precision;
	cv::Mat table;
	double sum;
	double hf_qval;
	double im_qval;
};

#endif
#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

#include <opencv2/core/core.hpp>

struct analysis {
	std::string type;
	cv::Mat image;
	std::string filename;
	std::string title;
};

struct qtable {
	int index;
	int precision;
	cv::Mat table;
	double sum;
	double hf_qval;
	double im_qval;
};

#endif
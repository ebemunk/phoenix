#include <opencv2/core/core.hpp>

using namespace cv;

void rgb_borders(Mat &src);

void hsv_histogram(Mat &src, Mat &dst, bool whitebg = false);

void lab_histogram(Mat &src, Mat &dst, bool whitebg = false);

void error_level_analysis(Mat &src, Mat &dst, int quality = 90);

void luminance_gradient(Mat &src, Mat &dst);

void avgdist(Mat &src, Mat &dst, double scale=5.0);
#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <opencv2/core/core.hpp>

#include "structs.h"

using namespace cv;

/*
	Return all colors which have at least one component (R,G,B) set to 255
*/	
void rgb_borders(Mat &dst);

/*
	HSV Histogram Stretch (Auto-Levels)
	converts the image to HSV colorspace and then applies histogram equalization
	to the V channel, and converts back to RGB. This is used to make copies that
	are better viewable
*/
void hsv_histogram_stretch(Mat &src, Mat &dst);

/*
	HSV Colorspace Histogram for the image. Count all occurrences of (H,S) and sum the V component, representing the average V in HSV colorspace for each color.
*/
void hsv_histogram(Mat &src, Mat &dst, bool whitebg = false);

/*
	Lab Colorspace Histogram for the image. Count all occurrences of (a,b) and sum the L component, representing the average L in Lab colorspace for each color.
*/
void lab_histogram(Mat &src, Mat &dst, bool whitebg = false);
void lab_histogram_fast(Mat &src, Mat &dst, bool whitebg = false);

/*
	Apply Error Level Analysis to the image. Resave source image at a known quality and subtract the known quality from the source image.
*/
void error_level_analysis(Mat &src, Mat &dst, int quality = 90);

/*
	Colorized X and Y Sobel filters.
*/
void luminance_gradient(Mat &src, Mat &dst);

/*
	Turn every pixel value to the average of the magnitude of its cross-shaped neighbors.
*/
void average_distance(Mat &src, Mat &dst);

/*
	Estimate JPEG quality using Hackerfactor and Imagemagick estimates
*/
int estimate_jpeg_quality(const char* filename, vector<qtable> &qtables, vector<double> &quality_estimates);

/*
	Copy-Move detection using DCT
*/
void copy_move_dct(Mat &src, Mat &dst, int retain = 4, double qcoeff = 1.0);

#endif
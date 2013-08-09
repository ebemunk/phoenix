#include <opencv2/core/core.hpp>

using namespace cv;

/**
 * Return all colors which have at least one component (R,G,B) set to 255
 * @param {Mat} dst Destination Mat to be filled
 */	
void rgb_borders(Mat &dst);

/**
 * HSV Colorspace Histogram for the image. Count all occurrences of (H,S) and sum the V component, representing the average V in HSV colorspace for each color.
 * @param {Mat} src Source image
 * @param {Mat} dst Destination image
 * @param {bool} whitebg White background? (default black)
 */
void hsv_histogram(Mat &src, Mat &dst, bool whitebg = false);

/**
 * Lab Colorspace Histogram for the image. Count all occurrences of (a,b) and sum the L component, representing the average L in Lab colorspace for each color.
 * @param {Mat} src Source image
 * @param {Mat} dst Destination image
 * @param {bool} whitebg White background? (default black)
 */
void lab_histogram(Mat &src, Mat &dst, bool whitebg = false);

/**
 * Apply Error Level Analysis to the image. Resave source image at a known quality and subtract the known quality from the source image.
 * @param {Mat} src Source image
 * @param {Mat} dst Destination image
 * @param {int} quality Resave quality [0,100]
 */
void error_level_analysis(Mat &src, Mat &dst, int quality = 90);

/**
 * Colorized X and Y Sobel filters.
 * @param {Mat} src Source image
 * @param {Mat} dst Destination image
 */
void luminance_gradient(Mat &src, Mat &dst);

/**
 * Turn every pixel value to the average of the magnitude of its cross-shaped neighbors.
 * @param {Mat} src Source image
 * @param {Mat} dst Destination image
 */
void average_distance(Mat &src, Mat &dst);
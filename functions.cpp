#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void rgb_borders(Mat &src) {
	src = Mat::zeros(3, 256*256, CV_8UC3);
	int b = 255, g=0, r=0;
	for(int g=0; g<256; g++) {
		for(int r=0; r<256; r++) {
			src.at<Vec3b>(0, g*256+r) = Vec3b(b, g, r);
		}
	}
	b=0;g=255;r=0;
	for(int b=0; b<256; b++) {
		for(int r=0; r<256; r++) {
			src.at<Vec3b>(1, b*256+r) = Vec3b(b, g, r);
		}
	}
	b=0;g=0;r=255;
	for(int b=0; b<256; b++) {
		for(int g=0; g<256; g++) {
			src.at<Vec3b>(2, b*256+g) = Vec3b(b, g, r);
		}
	}
}

void hsv_histogram(Mat &src, Mat &dst, bool whitebg = false) {
	Vec3f bgcolor = Vec3f(0,0,0);
	if(whitebg) {
		bgcolor = Vec3f(0,0,1);
	}
	Mat hsv;
	src.convertTo(hsv, CV_32F, 1.0/255.0);
	cvtColor(hsv, hsv, CV_BGR2HSV);
	//H: (0, 360) S: (0, 1) V: (0, 1)

	//count and calculate average V for each (H,S)
	int hbins = 360, sbins = 256;
	Mat hist = Mat::zeros(sbins, hbins, CV_32F);
	Mat sums = Mat::zeros(sbins, hbins, CV_32F);
	for(int i=0; i<src.rows; i++) {
		for(int j=0; j<src.cols; j++) {
			Vec3f pixel = hsv.at<Vec3f>(i,j);
			int H = round(pixel[0]), S = round(pixel[1]*255);
			hist.at<float>(S, H)++;
			sums.at<float>(S, H) += pixel[2];
		}
	}

	divide(sums, hist, hist);

	//draw histogram
	Mat hsv_histogram = Mat::zeros(sbins, hbins, CV_32FC3);
	for(int s=0; s<sbins; s++) {
		for(int h=0; h<hbins; h++) {
			float avg = hist.at<float>(s,h);
			if(avg > 0) {
				hsv_histogram.at<Vec3f>(s, h) = Vec3f(h, s/255.0, avg);
			} else {
				hsv_histogram.at<Vec3f>(s, h) = bgcolor;
			}
		}
	}

	//back to 8-bit rgb
	cvtColor(hsv_histogram, hsv_histogram, CV_HSV2BGR);
	hsv_histogram.convertTo(dst, CV_8U, 255);
}

void lab_histogram(Mat &src, Mat &dst, bool whitebg = false) {
	Vec3f bgcolor = Vec3f(0,0,0);
	if(whitebg) {
		bgcolor = Vec3f(100,0,0);
	}
	//convert to float and scale to [0,1]
	Mat lab;
	src.convertTo(lab, CV_32F, 1.0/255.0);
	cvtColor(lab, lab, CV_BGR2Lab);

	int abins = 1024, bbins = 1024;
	//count frequencies and also sum L values
	Mat hist = Mat::zeros(abins, bbins, CV_32F);
	Mat sums = Mat::zeros(abins, bbins, CV_32F);
	for(int i=0; i<src.rows; i++) {
		for(int j=0; j<src.cols; j++) {
			Vec3f pixel = lab.at<Vec3f>(i,j);
			int A = round(4*(pixel[1]+128)), B = round(4*(pixel[2]+128));
			hist.at<float>(A, B)++;
			sums.at<float>(A, B) += pixel[0];
		}
	}

	//get average L value for each bin
	divide(sums, hist, hist);

	//construct histogram image
	int sub = 512;
	Mat lab_histogram = Mat::zeros(abins, bbins, CV_32FC3);
	for(int a=0; a<abins; a++) {
		for(int b=0; b<bbins; b++) {
			float avg = hist.at<float>(a,b);
			if(avg>0) {
				lab_histogram.at<Vec3f>(b, a) = Vec3f(avg, (a-sub), (b-sub));
			} else {
				lab_histogram.at<Vec3f>(b, a) = bgcolor;
			}
		}
	}

	//back to 8-bit rgb
	cvtColor(lab_histogram, lab_histogram, CV_Lab2BGR);
	lab_histogram.convertTo(dst, CV_8U, 255);
}

void error_level_analysis(Mat &src, Mat &dst, int quality = 90) {
	vector<uchar> buffer;

    vector<int> save_params(2);
    save_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    save_params.push_back(quality);

	imencode(".jpg", src, buffer, save_params);

	Mat resaved = imdecode(buffer, CV_LOAD_IMAGE_COLOR);

	normalize(abs(src - resaved), dst, 0, 255, CV_MINMAX);
}

void luminance_gradient(Mat &src, Mat &dst) {
	Mat greyscale;
	cvtColor(src, greyscale, CV_BGR2GRAY);

	//get sobel in x and y directions
	Size size = src.size();
	Mat sobelX = Mat::zeros(size, CV_32F);
	Mat sobelY = Mat::zeros(size, CV_32F);

	Sobel(greyscale, sobelX, CV_32F, 1, 0);
	Sobel(greyscale, sobelY, CV_32F, 0, 1);

	dst = Mat::zeros(size, CV_32FC3);
	Vec3f lg_px;
	float sx, sy, angle;
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			sx = sobelX.at<float>(i, j);
			sy = sobelY.at<float>(i, j);
			angle = atan2(sx, sy);
			lg_px[0] = sqrt(pow(sx, 2) + pow(sy, 2)); //B: magnitude of the x and y derivatives
			lg_px[1] = -sin(angle) / 2.0 + 0.5; //G: -sin(angle) mapped to [0,1]
			lg_px[2] = -cos(angle) / 2.0 + 0.5; //R: -cos(angle) mapped to [0,1]
			dst.at<Vec3f>(i, j) = lg_px;
		}
	}

	//normalize and scale B channel to [0,1]
	vector<Mat> ch;
	split(dst, ch);
		normalize(ch[0], ch[0], 0, 1, CV_MINMAX);
	merge(ch, dst);

	dst.convertTo(dst, CV_8U, 255);
}

void avgdist(Mat &src, Mat &dst, double scale=5.0) {
	//average of cross-shaped neighbors filter
	Matx33f filter(0, 0.25, 0,
			0.25, 0, 0.25,
			0, 0.25, 0);

	src.convertTo(dst, CV_32F, 1.0/255.0);

	//apply filter
	Mat filtered;
	filter2D(dst, filtered, CV_32F, filter);
	normalize(abs(dst - filtered), dst, 0, 1, CV_MINMAX);
	dst.convertTo(dst, CV_8U, 255);
}
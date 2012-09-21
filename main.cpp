#include <iostream>

#define cimg_verbosity 0
#include "CImg.h"
#include "Util.h"

#include "MinMax.h"
#include "Histogram.h"

#include "Test.h"
#include <time.h>

using namespace std;
using namespace cimg_library;

void main(int argc, char* argv) {
	CImg<unsigned int> testimage("testimage.jpg");
	/* basic init tests
	(MinColor(testimage), MidColor(testimage), MaxColor(testimage)).display();
	test_MinMax1(testimage);
	test_MinMax3(testimage);
	test_All_display(testimage);
	*/
	//CImg<double> (*func)(CImg<unsigned int> &source); //function pointer
	/* test_speed function usage
	func = MinMax1;
	test_speed(func, testimage, 250);
	test_speed(func, testimage, 750);
	*/
	//(HSV_histogram(testimage, 666), rgb2gray(HSV_full_spectrum(666))).display("HSV | Full Spectrum");
	//CImg<double> hsv = HSV_histogram(testimage);
	//CImg<double> fs = rgb2gray(HSV_full_spectrum());
	//overlay(hsv, fs, 0.7).display();]
	//Lab_histogram(testimage).save("lol.jpg");
	Lab_full_spectrum().display();
	std::system("pause");
}
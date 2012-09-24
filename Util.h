/*
imgPhoenix
Utility header

utility functions that the main program or any other component has to use
*/
#ifndef Util_h_ver
#define Util_h_ver 1

#include "CImg.h"

using namespace cimg_library;

/*
convert image to greyscale
formula from http://www.mathworks.com/help/toolbox/images/ref/rgb2gray.html
returns CImg<double> so be careful!
*/
template<typename T>
CImg<double> rgb2gray( CImg<T> &input ) {
	if( input.spectrum() != 3 ) return input;
	CImg<double> output( input.width(), input.height(), 1, 3 );
	output.fill(0);
	cimg_forXYC( input, x, y, c ) {
		output(x,y,c) = 0.2989 * input(x,y,0) + 0.5870 * input(x,y,1) + 0.1140 * input(x,y,2);
	}
	return output;
}

/*
overlay two images on top of each other using the alpha value given for the first image
*/
template<typename T>
CImg<T> overlay(CImg<T> &source1, CImg<T> &source2, double alpha_s1=0.5) {
	CImg<T> output(source1, "xyzc");
	cimg_forXY(output, x, y) {
		output(x,y,0) = (alpha_s1 * source1(x,y,0)) + ((1-alpha_s1) * source2(x,y,0));
		output(x,y,1) = (alpha_s1 * source1(x,y,1)) + ((1-alpha_s1) * source2(x,y,1));
		output(x,y,2) = (alpha_s1 * source1(x,y,2)) + ((1-alpha_s1) * source2(x,y,2));
	}
	return output;
}
#endif
 
/*
MinMax Functions

These are a couple functions that used to reside in http://infohost.nmt.edu/~schlake/ela/ some time ago.
The page is inacessible now, I don't know what happened to it.
They were inside hfalg.c, ver 1.20 written by William D. Colburn (2010/10/08 04:23:00).
hfalg stands for Hacker Factor Algorithms.

Some functions below have been adapted from the original foruse with the CImg library.

See http://www.hackerfactor.com/papers/bh-usa-07-krawetz-wp.pdf for more information.
*/
#ifndef MinMax_h_ver
#define MinMax_h_ver 1

#include <algorithm>
#include "CImg.h"

using namespace cimg_library;

/*
the channel with the minimum color value is preserved
all the other channels are assigned 0
ties are not broken (> 1 channel might be min)
*/
template<typename T>
CImg<T> MinColor(CImg<T> &source) {
	//returned image same dimensions as source
	CImg<T> output( source, "xyzc" ); //fetch all dimensions of source image
	cimg_forXY(source, x, y) {
		T red = source(x,y,0);
		T green = source(x,y,1);
		T blue = source(x,y,2);
		T mincolor = min(red, min(green, blue));
		output(x,y,0) = red * (red == mincolor); //casts to 0 or 1
		output(x,y,1) = green * (green == mincolor);
		output(x,y,2) = blue * (blue == mincolor);
	}
	return output;
}

/*
returns the value in the middle (not max nor min)
*/
template<typename T>
T getMiddleVal(T a, T b, T c) {
	if(a<b) swap(a,b); // a > b
	if(b>c) return b; // b < c therefore
	if(a>c) return c;
	return a;
}

/*
outputs an image containing the middle value for every pixel
among all the channels. no ties are broken
*/
template<typename T>
CImg<T> MidColor(CImg<T> &source) {
	CImg<T> output( source, "xyzc");
	cimg_forXY(source, x, y) {
		T red = source(x,y,0);
		T green = source(x,y,1);
		T blue = source(x,y,2);
		T middle = getMiddleVal(red, green, blue);
		output(x,y,0) = red * (red == middle);
		output(x,y,1) = green * (green == middle);
		output(x,y,2) = blue * (blue == middle);
	}
	return output;
}

/*
outputs an image that contains only the maximum value for each channel
for every pixel. ties are not broken
*/
template<typename T>
CImg<T> MaxColor(CImg<T> &source) {
	CImg<T> output( source, "xyzc" ); //fetch all dimensions of source image
	cimg_forXY(source, x, y) {
		T red = source(x,y,0);
		T green = source(x,y,1);
		T blue = source(x,y,2);
		T maxcolor = max(red, max(green, blue));
		output(x,y,0) = red * (red == maxcolor);
		output(x,y,1) = green * (green == maxcolor);
		output(x,y,2) = blue * (blue == maxcolor);
	}
	return output;
}

/*
outputs 255 (max value) to a channel if it has a value that is greater
than the values of one of the other channels,
very similar to MaxColor, but with an OR instead of AND
returned image type is unsigned int, as 255 is used as max value for RGB
*/
template<typename T>
CImg<unsigned int> MinMax1(CImg<T> &source) {
	CImg<unsigned int> output( source, "xyzc" ); //fetch all dimensions of source image
	cimg_forXY(source, x, y) {
		T red = source(x,y,0);
		T green = source(x,y,1);
		T blue = source(x,y,2);
		output(x,y,0) = 255 * (red > green || red > blue);
		output(x,y,1) = 255 * (green > red || green > blue);
		output(x,y,2) = 255 * (blue > red || blue > green);
	}
	return output;
}

/*
color pixel red+green if its vector magnitude is the highest among its cross-shaped
neighbors. otherwise, if its vector magnitude is the lowest among its cross-shaped
neighbors, color it green+blue
returns unsigned int RGB image
*/
template<typename T>
CImg<unsigned int> MinMax3(CImg<T> &source) {
	CImg<unsigned int> output(source, "xyzc");
	//this next step saved about 2 seconds with a 1680x1024 test image
	//instead of calculating norm for every pixel iteration, otherwise you
	//actually compute the whole image more than once
	CImg<T> magnitude = source.get_norm();
	T min, max, current, temp;
	cimg_forXY(magnitude, x, y) {
		min = 255;
		max = 0;
		current = magnitude(x,y);
		if(x>0) { //left
			temp = magnitude(x-1,y);
			if(temp < min) min = temp;
			if(temp > max) max = temp;
		}
		if(x<source.width()-1) { //right
			temp = magnitude(x+1,y);
			if(temp < min) min = temp;
			if(temp > max) max = temp;
		}
		if(y>0) { //bottom
			temp = magnitude(x,y-1);
			if(temp < min) min = temp;
			if(temp > max) max = temp;
		}
		if(y<source.height()-1) { //top
			temp = magnitude(x,y+1);
			if(temp < min) min = temp;
			if(temp > max) max = temp;
		}
		//color pixel based on criteria
		if( current > max ) {
			output(x,y,0) = 255;
			output(x,y,1) = 255;
		} else if( current < min ) {
			output(x,y,1) = 255;
			output(x,y,2) = 255;
		}
	}
	return output;
}

/*
poorly-named algorithm, I'm guessing it means the average distance
but it returns the absolute difference between a pixel's channel value
and the average of all its neighbors (up, down, left, right). I'm not sure
where distance comes in to play, perhaps a loosely defined color distance?

I have also seen this analysis in Neal Krawetz's analysis blog, mentioned as
'Demosaic Modification Analysis'. http://www.hackerfactor.com/blog

returns float image type as we are dealing with floats. must be normalized 0-255 for viewing/saving
*/
template<typename T>
CImg<float> AvgDist(CImg<T> &source) {
	CImg<float> output(source, "xyzc");
	//create a filter that will take the average of a pixel's neighbors
	CImg<float> average_mask(3,3,1,1,0,1,0,1,0,1,0,1,0);
	average_mask /= (float) 4;
	//convolve image with source to get comparison image
	CImg<float> average = source.get_convolve(average_mask);
	cimg_forXYC(source, x, y, c) { //loop through X, Y and Channels
		output(x,y,c) = abs(source(x,y,c) - average(x,y,c));
	}
	return output;
}


#endif
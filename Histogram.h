
#ifndef Histogram_h_ver
#define Histogram_h_ver 1

#include "CImg.h"

using namespace cimg_library;

/*
displays a histogram of HSV values for an image. Hue/Saturation/Value is a colorspace. It has
cylindrical shape and this histogram sort of 'unfolds' the cylinder into a 2D square. X axis
represents the Hue and Y axis represents Saturation. The lightness of the color within a pixel
represents its Value.

more info about HSV colorspace: http://en.wikipedia.org/wiki/HSL_and_HSV
this function was adapted from: Samuel Albrecht's GIMP plugin code found at https://sites.google.com/site/elsamuko/forensics/hsv-analysis
speacial thanks to Neal Krawetz: http://www.hackerfactor.com/blog

this function assumes the image is in RGB format, and returns a double type image,
so take care to normalize when saving/displaying
*/
template<typename T>
CImg<double> HSV_histogram(CImg<T> &source, int bins = 256) {
	CImg<double> hsv_histogram(bins, bins, 1, 2); //histogram data
	CImg<double> source_hsv = source.get_RGBtoHSV(); //convert to HSV colorspace
	CImg<> H, S, V;
	//we scale the H and S channels between 0 and bins
	//this will be how we separate the 'bins' for the histogram
	//so H,S will give us the coordinates in the bins*bins histogram image
	H = source_hsv.get_channel(0).normalize(0, (bins-1));
	S = source_hsv.get_channel(1).normalize(0, (bins-1));
	V = source_hsv.get_channel(2); //don't scale the Value

	hsv_histogram.fill(0); //set all counts to 0

	cimg_forXY(source_hsv,x,y) {
		//notice how we use H and S values as X-Y coordinates, very cool!
		hsv_histogram((int) H(x,y), (int) S(x,y), 0) += V(x,y); //add to value counter
		hsv_histogram((int) H(x,y), (int) S(x,y), 1) ++; //add to frequency counter
	}

	cimg_forXY(hsv_histogram,x,y) {
		if(hsv_histogram(x,y,1) > 0) { //check to avoid diving by 0 in case there are none for that color
			hsv_histogram(x,y,0) = hsv_histogram(x,y,0) / hsv_histogram(x,y,1);
		}
		if( hsv_histogram(x,y,0) > 1 ) { //chek to make sure Value is within bounds
			hsv_histogram(x,y,0) = 1;
		}
	}

	CImg<double> output(bins,bins,1,3); //this will be the 'image' for the histogram
	cimg_forXY(output,x,y) {
		output(x,y,0) = x/(double)(bins-1)*360.0; //scale Hue to fit between bins pixels
		output(x,y,1) = y/(double)(bins-1); //scale Saturation, cast double so division doesn't mess up
		output(x,y,2) = hsv_histogram(x,y); //put Value as the Value channel
	}

	output.HSVtoRGB(); //convert to RGB for displaying and saving
	/*
	//leaving this loop out will make the background of the image (colors with 0 frequency) black
	//I think colors are identified easier on a black background (especially darker colors which contrast
	//with the pure white background
	cimg_forXY(output,x,y) { //just cosmetic, to get rid of the black background.
		if( output(x,y,0) == 0 && output(x,y,1) == 0 && output(x,y,2) == 0 ) {
			output(x,y,0) = output(x,y,1) = output(x,y,2) = 255;
		}
	}
	*/
	return output.mirror('y'); //mirror vertically for visual purposes
}

/*
returns an HSV histogram as if all rgb colors exist in the same quantity with Value = 1
effectively displaying the full color spectrum
*/
CImg<double> HSV_full_spectrum(int bins=256);

/*
displays a histogram of Lab values for image. for better details check out comment above HSV_histogram
they're pretty much the same.

more info about Lab colorspace: http://en.wikipedia.org/wiki/Lab_color_space
this function was adapted from: Samuel Albrecht's GIMP plugin code found at https://sites.google.com/site/elsamuko/forensics/lab-analysis

assumes source is in RGB, returns T = double
*/
template<typename T>
CImg<double> Lab_histogram(CImg<T> &source, int bins = 1024) {
	CImg<double> lab_histogram(bins, bins, 1, 2); //similar to the HSV procedure above, this table holds values and frequencies
	lab_histogram.fill(0); //initialize them to 0
	CImg<double> source_lab = source.get_RGBtoLab(); //convert to Lab
	// a and b components of Lab have range [-128, 128]
	// both for visualization and data storing, we have to scale and shift the whole thing
	// to fit the image boundaries. offset determines how much to shift right (in other words map [-128,128] to [0,256])
	// scalar determines how much to scale the histogram image
	double offset = bins / 2.0;
	double scalar = offset / 128.0;
	cimg_forXY(source,x,y) {
		double L;
		int a, b;
		L = source_lab(x,y,0);
		// again like HSV, we will use a and b components as indexes in the histogram table
		a = (int) (scalar*source_lab(x,y,1)+offset);
		b = (int) (scalar*source_lab(x,y,2)+offset);
		lab_histogram(a,b,0) += L; //increment value by L
		lab_histogram(a,b,1) ++; //increment frequency
	}

	cimg_forXY(lab_histogram,x,y) {
		if( lab_histogram(x,y,1) > 0 ) //make sure not dividing by 0
			lab_histogram(x,y,0) = lab_histogram(x,y,0) / lab_histogram(x,y,1);
	}

	CImg<double> output(bins,bins,1,3); //the actual image to return

	cimg_forXY(output,x,y) {
		if( lab_histogram(x,y,1) > 0 ) { //if frequency > 0 then color the pixel
			//we have to subtract the offset this time instead of adding
			//which reverses the translation at the beginning
			output(x,y,0) = lab_histogram(x,y,0);
			output(x,y,1) = x-offset;
			output(x,y,2) = y-offset;
		} else { //otherwise leave it black
			output(x,y,0) = 0;
			output(x,y,1) = 0;
			output(x,y,2) = 0;
		}
	}

	output.LabtoRGB();

	return output;
}

CImg<double> Lab_full_spectrum(int bins=1024);
#endif
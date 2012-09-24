/*
imgPhoenix
Histograms cpp

This file contains functions that are not template declarations, namely
HSV_full_spectrum creates the full rgb spectrum on the HSV histogram, and 
Lab_full_spectrum does the same on Lab colorspace. These are useful when displaying
the histograms along with a 'border' of the RGB space for better visualization.

NOTE: Lab_full_spectrum is really slow, so Histogram display function will first look for
an already-computed copy before invoking this function.
*/
#include "Histogram.h"
#include "CImg.h"
#include "Util.h"
#include <iostream>
#include <string>

using namespace cimg_library;

/*
returns an HSV histogram as if all rgb colors exist in the same quantity with Value = 1
effectively displaying the full color spectrum
*/
CImg<double> HSV_full_spectrum(int bins) {

	CImg<double> output(bins,bins,1,3); //this will be the 'image' for the histogram
	cimg_forXY(output,x,y) {
		output(x,y,0) = x/(double)(bins-1)*360.0; //scale Hue to fit between bins pixels
		output(x,y,1) = y/(double)(bins-1); //scale Saturation, cast double so division doesn't mess up
		output(x,y,2) = 1; //put Value as 1
	}

	output.HSVtoRGB(); //convert to RGB for displaying and saving

	return output.mirror('y'); //mirror vertically for visual purposes
}

/*
returns the Lab histogram of all RGB colors. this is to display the colorspace boundaries
(i.e. RGB colorspace  inside Lab). this function is really inefficient, so if overlay is chosen
phoenix will first attempt to get the already-saved template for it
*/
CImg<double> Lab_full_spectrum(int bins) {

	CImg<double> output(bins, bins, 1, 3);
	output.fill(0);

	//same deal as in Lab histogram function
	double scale, offset;
	offset = bins / 2.0;
	scale = offset / 128.0;

	CImg<double> pixel(1,1,1,3); //hold the single pixel value
	//loop through all R G B channels and mark the locations of each in Lab space
	for(int i=0; i<256; i++) { //r
		for(int j=0; j<256; j++) { //g
			for(int k=0; k<256; k++) { //b
				pixel(0) = i;
				pixel(1) = j;
				pixel(2) = k;
				pixel.RGBtoLab(); //get lab value of pixel
				int a = (int)(scale*pixel(1)+offset);
				int b = (int)(scale*pixel(2)+offset);
				output(a,b,0) = 1; //we use red channel to 'mark' the locations
			}
		}
	}

	//loop through marked locations and color in those pixels
	cimg_forXY(output,x,y) {
		if(output(x,y,0) > 0) {
			output(x,y,0) = 100;
			output(x,y,1) = x-offset;
			output(x,y,2) = y-offset;
		}
	}

	output.LabtoRGB(); //back to rgb

	return output;
}

/*
returns the borders of the RGB colorspace in any one of the histograms determined by type.
overlays the rgb colorspace with the data from the histogram for better visualization
*/
CImg<double> colorspace_borders(CImg<double> &histogram, int type) {
	CImg<double> spectrum; //full spectrum image
	if(type == HSV) {
		spectrum = HSV_full_spectrum(histogram.width());
	} else if(type == Lab) {
		// as Lab_full_spectrum takes some time, see if we can load it from disk
		//first construct filename for access or saving
		char *width;
		sprintf(width, "%d", histogram.width());
		std::string filename = "imgphoenix_resources/Lab_";
		filename += width;
		filename += "_fs.jpg";
        cimg::exception_mode(0); //set mode 0 so exception isn't handled by the CImgDisplay window
		try {
			spectrum = CImg<double>(filename.c_str());
		} catch( CImgException &e ) {
			//std::fprintf(stderr,"CImg Library Error : %s",e.what());
			//TODO: output expection information if verbose if enabled
			spectrum = Lab_full_spectrum(histogram.width());
			spectrum.save(filename.c_str());
		}
	}
	return overlay(histogram, spectrum, 0.95);
}
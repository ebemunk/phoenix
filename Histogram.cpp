#include "Histogram.h"
#define cimg_verbosity 0
#include "CImg.h"

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

//TODO: fix
CImg<double> Lab_full_spectrum(int bins) {
	
	CImg<double> output(bins,bins,1,3); //the actual image to return
	double offset = bins / 2.0;
	cimg_forXY(output,x,y) {
			//we have to subtract the offset this time instead of adding
			//which reverses the translation at the beginning
			output(x,y,0) = 200;
			output(x,y,1) = x-offset;
			output(x,y,2) = y-offset;
	}

	output.LabtoRGB();

	return output;
}
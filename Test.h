/*
imgPhoenix
tests functions
*/
#ifndef Test_h_ver
#define Test_h_ver 1

#include "CImg.h"

#include "MinMax.h"
#include "Histogram.h"

#include <time.h>

using namespace std;
using namespace cimg_library;

/* highly experimental */
template<typename T>
void _test( CImg<T>(*func)(CImg<T> &), CImg<T> &source ) {
	CImg<> img = (*func)(source);
	img.display();
}

/* good for speed testing in gui */
template<typename T>
void test_All_display(CImg<T> &source) {
	(source, MinColor(source), MidColor(source), MaxColor(source), MinMax1(source), MinMax3(source), AvgDist(source), HSV_histogram(source)).display("Original | MinColor | MidColor | MaxColor | MinMax1 | MinMax3 | AvgDist | HSV Histogram");
}

/*
generic speed test function that takes 3 params
1) pointer to a function you want to run
2) source image you want to use
3) number of iterations to run it for
the function simply displays how long it takes to run (*func) on source, num_itr times
the declaration looks so horrible, sorry!
*/
template<typename T>
void test_speed( CImg<T>(*func)(CImg<T> &), CImg<T> &source, int num_itr ) {
	time_t start, end;
	time(&start);
	for(int i=0; i<num_itr; i++) {
		(*func)(source);
	}
	time(&end);
	printf("Time elapsed: %.2lf \n", difftime(end, start));
	return;
}
#endif
 
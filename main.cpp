#include <iostream>

#include "CImg.h"

#include "MinMax.h"

#include "Test.h"
#include <time.h>

using namespace std;
using namespace cimg_library;

void main(int argc, char* argv) {
	CImg<unsigned int> testimage("testimage.jpg");
	//(MinColor(testimage), MidColor(testimage), MaxColor(testimage)).display();
	//test_MinMax1(testimage);
	//test_MinMax3(testimage);
	//test_All_display(testimage);
	CImg<unsigned int> (*func)(CImg<unsigned int> &source);
	//func = MinMax1;
	//test_speed(func, testimage, 250);
	//test_speed(func, testimage, 750);
	std::system("pause");
}
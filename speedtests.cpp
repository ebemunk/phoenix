#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <chrono>
#include <iostream>

#include "functions.hpp"

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
	Mat source_image;
	source_image = imread("C:\\wamp\\www\\pe\\source_images\\A.jpg", CV_LOAD_IMAGE_COLOR);
	
	cout << "Starting speed tests..." << endl;

	Mat tmp;
	
	auto start_time = chrono::high_resolution_clock::now();
		for(int i=0; i<10; i++) {
			error_level_analysis(source_image, tmp);
		}
	auto end_time = chrono::high_resolution_clock::now();
	cout << "ELA: ";
	cout << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << ":";
	cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << ":" << endl;
	
	start_time = chrono::high_resolution_clock::now();
		for(int i=0; i<10; i++) {
			luminance_gradient(source_image, tmp);
		}
	end_time = chrono::high_resolution_clock::now();
	cout << "LG: ";
	cout << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << ":";
	cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << ":" << endl;
	
	start_time = chrono::high_resolution_clock::now();
		for(int i=0; i<10; i++) {
			average_distance(source_image, tmp);
		}
	end_time = chrono::high_resolution_clock::now();
	cout << "Avgdist: ";
	cout << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << ":";
	cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << ":" << endl;
	
	start_time = chrono::high_resolution_clock::now();
		for(int i=0; i<10; i++) {
			hsv_histogram(source_image, tmp);
		}
	end_time = chrono::high_resolution_clock::now();
	cout << "HSV: ";
	cout << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << ":";
	cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << ":" << endl;
	
	start_time = chrono::high_resolution_clock::now();
		for(int i=0; i<10; i++) {
			lab_histogram(source_image, tmp);
		}
	end_time = chrono::high_resolution_clock::now();
	cout << "Lab: ";
	cout << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << ":";
	cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << ":" << endl;
	
	return 0;
}
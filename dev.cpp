#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <chrono>
#include <iostream>

#include "debugger.hpp"
#include "functions.hpp"

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
	debugger &d = debugger::instance();
	// d.active = true;

	d.start("test");
	d.end("test");
	d.print("hello");
}
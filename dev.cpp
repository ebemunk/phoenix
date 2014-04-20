#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <chrono>
#include <iostream>

#include "functions.hpp"

using namespace std;
using namespace cv;

template<class T> class sorter {
	private:
		const vector<T> &values;

	public:
		sorter(const vector<T> &v) : values(v) {}

		bool operator()(int a, int b) {
			unsigned char *v_a = (unsigned char*)(values[a].data);
			unsigned char *v_b = (unsigned char*)(values[b].data);

			int limit = values[a].cols * values[a].rows;

			return lexicographical_compare(v_a, v_a+limit, v_b, v_b+limit);
		}
};

// class debugger {
// 	public:
// 	chrono::system_clock::time_point t_start, t_end;
// 	static void start(string msg) {
// 		t_start = chrono::high_resolution_clock::now();
// 		cout << endl << msg << " starting" << endl;
// 	}
// 	static void end(string msg, bool print_time = true) {
// 		t_end = chrono::high_resolution_clock::now();

// 		int secs = chrono::duration_cast<chrono::seconds>(t_end - t_start).count();
// 		int millisecs = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count();
// 		cout << msg << " took: " << secs << ":" << millisecs << endl << endl;
// 	}
// };

int main(int argc, char *argv[]) {

	Mat src1;
	src1 = imread("C:\\wamp\\www\\phoenix\\i\\g.jpg", CV_LOAD_IMAGE_COLOR);
	Mat rectBuffer = src1.clone();

	Mat src;
	src = imread("C:\\wamp\\www\\phoenix\\i\\g.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	src.convertTo(src, CV_32F);

	int retain_subm = 3;
	int subm_limit = retain_subm * retain_subm;

	int blocksize = 16;
	int total_blocks = (src.rows - blocksize + 1) * (src.cols - blocksize + 1);

	int blocks_height = src.rows-blocksize+1;
	int blocks_width = src.cols-blocksize+1;

	vector< Mat > blocks;
	blocks.reserve(total_blocks);

	Mat tmp;

	for(int y=0; y<blocks_height; y++) {
		for(int x=0; x<blocks_width; x++) {
			dct(src(Rect(x,y,blocksize,blocksize)), tmp);
			// tmp = tmp / 40.5;
			tmp.convertTo(tmp, CV_8U);
			blocks.push_back(tmp(Rect(0,0,retain_subm, retain_subm)).clone());
		}
	}

	int *index = new int[total_blocks];
	for(int i=0; i<total_blocks; i++)
		index[i] = i;

	int *s_count = new int[src.rows * src.cols * 2];
	for (int i=0; i<src.rows * src.cols * 2; i++ )
		s_count[i] = 0;

	sort(index, index+total_blocks, sorter<Mat>(blocks));

	for(int i=0; i<total_blocks-1; i++) {
		unsigned char *v_a = (unsigned char*)(blocks[index[i]].data);
		unsigned char *v_b = (unsigned char*)(blocks[index[i+1]].data);

		if(equal(v_a, v_a+subm_limit, v_b)) {
			Point cur, next, shift;
			cur.x = index[i] % blocks_width;
			cur.y = (index[i] - cur.x) / (float)blocks_width;

			next.x = index[i+1] % blocks_width;
			next.y = (index[i+1] - next.x) / (float)blocks_width;

			shift = cur - next;
			if(shift.x < 0) shift *= -1;

			double magnitude = norm(shift);

			shift.y += src.rows;

			if ( magnitude > blocksize ) {
				int s_indx = shift.y * (src.cols) + shift.x;
				s_count[s_indx]++;
			}
		}
	}

	for(int i=0; i<total_blocks-1; i++) {
		unsigned char *v_a = (unsigned char*)(blocks[index[i]].data);
		unsigned char *v_b = (unsigned char*)(blocks[index[i+1]].data);

		if(equal(v_a, v_a+subm_limit, v_b)) {
			Point cur, next, shift;
			cur.x = index[i] % blocks_width;
			cur.y = (index[i] - cur.x) / (float)blocks_width;

			next.x = index[i+1] % blocks_width;
			next.y = (index[i+1] - next.x) / (float)blocks_width;

			shift = cur - next;
			if(shift.x < 0) shift *= -1;

			double magnitude = norm(shift);

			shift.y += src.rows;

			RNG rng((int)magnitude);
			Vec3b color = Vec3b(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));

			if( s_count[shift.y*src.cols+shift.x] > 10 ) {
				for(int ii=0; ii<blocksize; ii++) {
					for(int jj=0; jj<blocksize; jj++) {
							rectBuffer.at<Vec3b>(cur.y+ii, cur.x+jj) = color;
							rectBuffer.at<Vec3b>(next.y+ii, next.x+jj) = color;
					}
				}
			}
		}
	}

	addWeighted(src1, 0.2, rectBuffer, 0.8, 0, src1);

	imshow("A", src1);
	imshow("B", rectBuffer);
	// src.convertTo(src, CV_8U);

	waitKey(0);
}
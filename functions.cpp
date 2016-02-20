#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "structs.h"

using namespace std;
using namespace cv;
using boost::property_tree::ptree;

/*
	HSV Histogram Stretch (Auto-Levels)
	converts the image to HSV colorspace and then applies histogram equalization
	to the V channel, and converts back to RGB. This is used to make copies that
	are better viewable
*/
void hsv_histogram_stretch(Mat &src, Mat &dst) {
	//convert to float & HSV colorspace
	src.convertTo(dst, CV_32F, 1.0/255.0);
	cvtColor(dst, dst, CV_BGR2HSV);

	vector<Mat> ch;
	split(dst, ch);
		//convert V channel to 8-bit
		ch[2].convertTo(ch[2], CV_8U, 255);
		//equalize histogram
		equalizeHist(ch[2], ch[2]);
		//back to float & normalize to [0,1]
		ch[2].convertTo(ch[2], CV_32F, 1.0/255.0);
		normalize(ch[2], ch[2], 0, 1, CV_MINMAX);
	merge(ch, dst);

	//back to 8-bit rgb
	cvtColor(dst, dst, CV_HSV2BGR);
	dst.convertTo(dst, CV_8U, 255);
}

/*
	HSV Histogram Analysis
	convert image to float and change colorspace to HSV. Count all H,S pairs and
	compute frequency + the total value for V for each pair. Divide the total value
	by the frequency and create the histogram image

	implementation adapted from Samuel Albrecht's GIMP plugin
	https://sites.google.com/site/elsamuko/forensics/hsv-analysis
*/
void hsv_histogram(Mat &src, Mat &dst, bool whitebg = false) {
	Vec3f bgcolor = Vec3f(0,0,0);
	if(whitebg) {
		bgcolor = Vec3f(0,0,1);
	}
	Mat hsv;
	src.convertTo(hsv, CV_32F, 1.0/255.0);
	cvtColor(hsv, hsv, CV_BGR2HSV);
	//H: (0, 360) S: (0, 1) V: (0, 1)

	//count and calculate average V for each (H,S)
	int hbins = 360, sbins = 256;
	Mat hist = Mat::zeros(sbins, hbins, CV_32F);
	Mat sums = Mat::zeros(sbins, hbins, CV_32F);
	for(int i=0; i<src.rows; i++) {
		for(int j=0; j<src.cols; j++) {
			Vec3f pixel = hsv.at<Vec3f>(i,j);
			int H = round(pixel[0]), S = round(pixel[1]*255);
			hist.at<float>(S, H)++;
			sums.at<float>(S, H) += pixel[2];
		}
	}

	divide(sums, hist, hist);

	//draw histogram
	Mat hsv_histogram = Mat::zeros(sbins, hbins, CV_32FC3);
	for(int s=0; s<sbins; s++) {
		for(int h=0; h<hbins; h++) {
			float avg = hist.at<float>(s,h);
			if(avg > 0) {
				hsv_histogram.at<Vec3f>(s, h) = Vec3f(h, s/255.0, avg);
			} else {
				hsv_histogram.at<Vec3f>(s, h) = bgcolor;
			}
		}
	}

	//back to 8-bit rgb
	cvtColor(hsv_histogram, hsv_histogram, CV_HSV2BGR);
	hsv_histogram.convertTo(dst, CV_8U, 255);
}

/*
	Lab Histogram Analysis
	convert image to float and change colorspace to Lab. Count all a,b pairs and
	compute the frequency + total value for L for each pair. Divide & display the
	resulting histogram image

	implementation adapted from Samuel Albrecht's GIMP plugin
	https://sites.google.com/site/elsamuko/forensics/lab-analysis
*/
void lab_histogram(Mat &src, Mat &dst, bool whitebg = false) {
	Vec3f bgcolor = Vec3f(0,0,0);
	if(whitebg) {
		bgcolor = Vec3f(100,0,0);
	}
	//convert to float and scale to [0,1]
	Mat lab;
	src.convertTo(lab, CV_32F, 1.0/255.0);
	cvtColor(lab, lab, CV_BGR2Lab);
	//L: (0, 100) a: (-127, 127) b: (-127, 127)

	int abins = 1024, bbins = 1024;
	//count frequencies and also sum L values
	Mat hist = Mat::zeros(abins, bbins, CV_32F);
	Mat sums = Mat::zeros(abins, bbins, CV_32F);
	for(int i=0; i<src.rows; i++) {
		for(int j=0; j<src.cols; j++) {
			Vec3f pixel = lab.at<Vec3f>(i,j);
			int A = round(4*(pixel[1]+128)), B = round(4*(pixel[2]+128));
			hist.at<float>(A, B)++;
			sums.at<float>(A, B) += pixel[0];
		}
	}

	//get average L value for each bin
	divide(sums, hist, hist);

	//construct histogram image
	int sub = 512;
	Mat lab_histogram = Mat::zeros(abins, bbins, CV_32FC3);
	for(int a=0; a<abins; a++) {
		for(int b=0; b<bbins; b++) {
			float avg = hist.at<float>(a,b);
			if(avg>0) {
				lab_histogram.at<Vec3f>(b, a) = Vec3f(avg, (a-sub), (b-sub));
			} else {
				lab_histogram.at<Vec3f>(b, a) = bgcolor;
			}
		}
	}

	//back to 8-bit rgb
	cvtColor(lab_histogram, lab_histogram, CV_Lab2BGR);
	lab_histogram.convertTo(dst, CV_8U, 255);
}

/*
	Fast version of Lab Histogram, converting to Lab from CV_8U rather than
	CV_32F saves a ton of time, but its less accurate.
*/
void lab_histogram_fast(Mat &src, Mat &dst, bool whitebg = false) {
	Vec3f bgcolor = Vec3f(0,0,0);
	if(whitebg) {
		bgcolor = Vec3f(100,0,0);
	}
	//convert to float and scale to [0,1]
	Mat lab;
	// src.convertTo(lab, CV_32F, 1.0/255.0);
	cvtColor(src, lab, CV_BGR2Lab);

	lab.convertTo(lab, CV_32F);
	vector<Mat> chn;
	split(lab, chn);
		chn[0] = (chn[0] / 255.0) * 100.0;
		chn[1] = chn[1] - 128;
		chn[2] = chn[2] - 128;
	merge(chn, lab);
	//L: (0, 100) a: (-127, 127) b: (-127, 127)

	int abins = 256, bbins = 256;
	//count frequencies and also sum L values
	Mat hist = Mat::zeros(abins, bbins, CV_32F);
	Mat sums = Mat::zeros(abins, bbins, CV_32F);
	for(int i=0; i<src.rows; i++) {
		for(int j=0; j<src.cols; j++) {
			Vec3f pixel = lab.at<Vec3f>(i,j);
			int A = round(1*(pixel[1]+128)), B = round(1*(pixel[2]+128));
			hist.at<float>(A, B)++;
			sums.at<float>(A, B) += pixel[0];
		}
	}

	//get average L value for each bin
	divide(sums, hist, hist);

	//construct histogram image
	int sub = 128;
	Mat lab_histogram = Mat::zeros(abins, bbins, CV_32FC3);
	for(int a=0; a<abins; a++) {
		for(int b=0; b<bbins; b++) {
			float avg = hist.at<float>(a,b);
			if(avg>0) {
				lab_histogram.at<Vec3f>(b, a) = Vec3f(avg, (a-sub), (b-sub));
			} else {
				lab_histogram.at<Vec3f>(b, a) = bgcolor;
			}
		}
	}

	//back to 8-bit rgb
	cvtColor(lab_histogram, lab_histogram, CV_Lab2BGR);
	lab_histogram.convertTo(dst, CV_8U, 255);
}

/*
	Error Level Analysis
	encode a jpeg with a known quality (default 90) and then subtract this image
	from the original jpeg. Normalize the resulting image for better viewing

	implemented from Neal Krawetz's algorithm description
	http://hackerfactor.com/papers/bh-usa-07-krawetz-wp.pdf
	pages 16-20
*/
void error_level_analysis(Mat &src, Mat &dst, int quality = 90) {
	vector<uchar> buffer;

	vector<int> save_params(2);
	save_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	save_params.push_back(quality);
	//encode as jpeg
	imencode(".jpg", src, buffer, save_params);

	Mat resaved = imdecode(buffer, CV_LOAD_IMAGE_COLOR);
	//normalize the difference for better viewing
	normalize(abs(src - resaved), dst, 0, 255, CV_MINMAX);
}

/*
	Luminance Gradient
	get image derivatives in X and Y directions using a Sobel filter. afterwards,
	colorize the image using the X and Y sobel components as angle in G and R channels
	and magnitude of the vectors as the B channel.

	implemented from Neal Krawetz's algorithm description
	http://blackhat.com/presentations/bh-dc-08/Krawetz/Presentation/bh-dc-08-krawetz.pdf
	pages 60-72
*/
void luminance_gradient(Mat &src, Mat &dst) {
	Mat greyscale;
	cvtColor(src, greyscale, CV_BGR2GRAY);

	//get sobel in x and y directions
	Size size = src.size();
	Mat sobelX;
	Mat sobelY;

	Sobel(greyscale, sobelX, CV_32F, 1, 0);
	Sobel(greyscale, sobelY, CV_32F, 0, 1);

	dst = Mat::zeros(size, CV_32FC3);

	int rows = dst.rows;
	int cols = dst.cols;
	if(dst.isContinuous()) {
		cols = rows * cols;
		rows = 1;
	}

	for(int i=0; i<rows; i++) {
		Vec3f *ptr = dst.ptr<Vec3f>(i);
		float *sx = sobelX.ptr<float>(i);
		float *sy = sobelY.ptr<float>(i);
		for(int j=0; j<cols; j++) {
			float angle = atan2(sx[j], sy[j]);
			Vec3f pixel;
			pixel[0] = sqrt((sx[j]*sx[j]) + (sy[j]*sy[j])); //B: magnitude of the x and y derivatives
			pixel[1] = (-sin(angle) / 2.0 + 0.5); //G: -sin(angle) mapped to [0,1]
			pixel[2] = (-cos(angle) / 2.0 + 0.5); //R: -cos(angle) mapped to [0,1]
			ptr[j] = pixel;
		}
	}

	vector<Mat> ch;
	split(dst, ch);
		normalize(ch[0], ch[0], 0, 1, CV_MINMAX);
	merge(ch, dst);

	dst.convertTo(dst, CV_8U, 255);
}

/*
	Turn all pixels into the average of the magnitude of its cross-shaped neighbors.

	implemented from https://infohost.nmt.edu/~schlake/ela/src/hfalg.c
*/
void average_distance(Mat &src, Mat &dst) {
	//average of cross-shaped neighbors filter
	Matx33f filter(0, 0.25, 0,
			0.25, 0, 0.25,
			0, 0.25, 0);

	src.convertTo(dst, CV_32F, 1.0/255.0);

	//apply filter
	Mat filtered;
	filter2D(dst, filtered, CV_32F, filter);
	normalize(abs(dst - filtered), dst, 0, 1, CV_MINMAX);
	dst.convertTo(dst, CV_8U, 255);
}

/*
	Extract given marker from jpeg file.
*/
int extract_jpeg_marker(const char* filename, char marker, vector<char*> &list) {
	//open file and get started
	ifstream in(filename, ios::binary);

	// first two bytes must be 0xffd8 for jpeg format
	char buffer[2];
	in.read(buffer, 2);
	if(buffer[0] != (char)0xFF && buffer[1] != (char)0xD8) {
		//not jpeg
		return -2;
	}
	/*cout << "First Two: " << endl;
	cout << "\t" << hex << (unsigned short)buffer[0] << endl;
	cout << "\t" << hex << (unsigned short)buffer[1] << endl;*/
	
	in.read(buffer, 2);
	if(buffer[0] != (char)0xFF) {
		//jpeg but corrupt?
		return -1;
	}
	/*cout << "Third: ";
	cout << "\t" << hex << (unsigned short)buffer[0] << endl;*/

	/**
	 * loop until:
	 * - end of file
	 * - end of image 0xd9
	 * - hit image data (no headers after image data starts)
	 */
	bool compressed = false;
	while(buffer[1] != (char)0xD9 && !compressed && in.tellg() != -1) {
		/*cout << "Marker:\t" << hex << (unsigned short)buffer[1] << endl;*/
		//check that segment marker is not a restart marker
		if(buffer[1] < (char)0xD0 || buffer[1] > (char)0xD7) {
			//next two bytes are the size of the segment
			char size[2];
			in.read(size, 2);
			/*cout << "Size:\t" << hex << (unsigned short)size[0] << endl << "\t" << hex << (unsigned short)size[1] << endl;*/
			//convert to short
			unsigned short size_s = size[0];
			size_s <<= 8;
			size_s |= size[1] & 0x00FF; //this last bit mask was what was missing all along! why this way?

			/*cout << "+-+-+-+-+-+-+-+-+-+" << endl;
			cout << "Size: " << dec << size_s << endl;*/

			//read segment
			//segment size includes the previous two size bytes (i think)
			char *segdata = new char[size_s-2];
			in.read(segdata, size_s-2);

			//DQT marker 0xdb
			if(buffer[1] == marker) {
				list.push_back(segdata);
			}
			/*cout << "+-+-+-+-+-+-+-+-+-+" << endl;*/
		}

		//if we see start of scan (SOS 0xda) that means its just image data from here on
		if(buffer[1] == (char)0xDA) {
			compressed = true;
		} else {
			//read the next two bytes, first one must be 0xff start of segment
			in.read(buffer, 2);
			if(buffer[0] != (char)0xFF) { //something wrong with this jpeg
				return -1;
			}
		}
	} //file reading complete

	return list.size();
}

/*
	Estimate jpeg quality from extracted QTs (Quantization Tables)

	uses estimation method in Neal Krawetz's jpegquality tool
	http://www.hackerfactor.com/src/jpegquality.c

	also uses estimation tables from Imagemagick codebase
	http://trac.imagemagick.org/browser/ImageMagick/trunk/coders/jpeg.c
*/
int estimate_jpeg_quality(const char* filename, vector<qtable> &qtables, vector<double> &quality_estimates) {
	vector<char*> dqt_tables;

	int num_segments = extract_jpeg_marker(filename, 0xDB, dqt_tables);
	if(num_segments < 1) {
		return num_segments;
	}

	Mat zigzag8 = (Mat_<int>(64, 1) << 0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42, 3, 8, 12, 17, 25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63);
	
	//loop over extracted files and prepare to estimate quality
	for(int k=0; k<dqt_tables.size(); k++) {
		Mat dqt(8,8, CV_32F);
		//precision and index is packed into this first byte
		char precision_index = dqt_tables[k][0];
		int precision, index;
		index = precision_index & 0x0F; //first 4 bits
		precision = (precision_index & 0xF0) & 0x0F; //last 4 bits

		//load the rest of the segment data to DQT matrix - in zigzag order
		for(int i=0; i<8; i++) {
			for(int j=0; j<8; j++) {
				//dqt.at<float>(i, j) = segdata[i*8+j]; //non-zigzag order
				dqt.at<float>(i, j) = dqt_tables[k][zigzag8.at<int>(i*8+j)+1];
			}
		}
		CvScalar sum = cv::sum(dqt);
		//Hacker Factor quality estimate for table
		double hf_qval = 100 - ((sum.val[0] - dqt.at<float>(0, 0)) / 63.0);
		//ImageMagick initial qval
		double im_qval;
		if(k==1) {
			im_qval = dqt_tables[k][2] + dqt_tables[k][53];
		} else {
			im_qval = dqt_tables[k][0] + dqt_tables[k][63];
		}

		//push it to vector
		qtable table = {
			index, precision, dqt, sum.val[0], hf_qval, im_qval
		};
		qtables.push_back(table);
	}

	double hf_quality, imagick_quality;
	int num_qtables = 0;

	//jpeg with only 1 quantization table
	if(dqt_tables.size() == 1) {
		num_qtables = 1;
		//hackerfactor estimate
		hf_quality = qtables[0].hf_qval;

		//imagemagick estimation tables for single-dqt jpgs
		size_t
		hash[101] = {
			510, 505, 422, 380, 355, 338, 326, 318, 311, 305,
			300, 297, 293, 291, 288, 286, 284, 283, 281, 280,
			279, 278, 277, 273, 262, 251, 243, 233, 225, 218,
			211, 205, 198, 193, 186, 181, 177, 172, 168, 164,
			158, 156, 152, 148, 145, 142, 139, 136, 133, 131,
			129, 126, 123, 120, 118, 115, 113, 110, 107, 105,
			102, 100, 97, 94, 92, 89, 87, 83, 81, 79,
			76, 74, 70, 68, 66, 63, 61, 57, 55, 52,
			50, 48, 44, 42, 39, 37, 34, 31, 29, 26,
			24, 21, 18, 16, 13, 11, 8, 6, 3, 2,
			0
		},
		sums[101] = {
			16320, 16315, 15946, 15277, 14655, 14073, 13623, 13230, 12859,
			12560, 12240, 11861, 11456, 11081, 10714, 10360, 10027, 9679,
			9368, 9056, 8680, 8331, 7995, 7668, 7376, 7084, 6823,
			6562, 6345, 6125, 5939, 5756, 5571, 5421, 5240, 5086,
			4976, 4829, 4719, 4616, 4463, 4393, 4280, 4166, 4092,
			3980, 3909, 3835, 3755, 3688, 3621, 3541, 3467, 3396,
			3323, 3247, 3170, 3096, 3021, 2952, 2874, 2804, 2727,
			2657, 2583, 2509, 2437, 2362, 2290, 2211, 2136, 2068,
			1996, 1915, 1858, 1773, 1692, 1620, 1552, 1477, 1398,
			1326, 1251, 1179, 1109, 1031, 961, 884, 814, 736,
			667, 592, 518, 441, 369, 292, 221, 151, 86,
			64, 0
		};

		//imagemagick estimate
		double sum = qtables[0].sum;
		double qvalue = qtables[0].im_qval;
		for (int i=0; i < 100; i++) {
			if ((qvalue < hash[i]) && (sum < sums[i])) {
				continue;
			}
			if (((qvalue <= hash[i]) && (sum <= sums[i])) || (i >= 50)) {
				imagick_quality = i + 1;
			}
			break;
		}
	} else { // 2 or 3 quantization tables
		if(dqt_tables.size() == 2) { //this means Cr and Cb tables are the same
			num_qtables = 2;
			qtables.push_back(qtables[1]);
		} else {
			num_qtables = 3;
		}

		//hackerfactor estimate
		double q0, q1, q2;
		q0 = qtables[0].hf_qval;
		q1 = qtables[1].hf_qval;
		q2 = qtables[2].hf_qval;
		double diff = (abs(q0 - q1) + abs(q0 - q2)) * 0.49;
		hf_quality = (q0 + q1 + q2) / 3.0 + diff;

		//imagemagick estimation tables for multi-dqt jpgs
		size_t
		hash[101] = {
			1020, 1015, 932, 848, 780, 735, 702, 679, 660, 645,
			632, 623, 613, 607, 600, 594, 589, 585, 581, 571,
			555, 542, 529, 514, 494, 474, 457, 439, 424, 410,
			397, 386, 373, 364, 351, 341, 334, 324, 317, 309,
			299, 294, 287, 279, 274, 267, 262, 257, 251, 247,
			243, 237, 232, 227, 222, 217, 213, 207, 202, 198,
			192, 188, 183, 177, 173, 168, 163, 157, 153, 148,
			143, 139, 132, 128, 125, 119, 115, 108, 104, 99,
			94, 90, 84, 79, 74, 70, 64, 59, 55, 49,
			45, 40, 34, 30, 25, 20, 15, 11, 6, 4,
			0
		},
		sums[101] = {
			32640, 32635, 32266, 31495, 30665, 29804, 29146, 28599, 28104,
			27670, 27225, 26725, 26210, 25716, 25240, 24789, 24373, 23946,
			23572, 22846, 21801, 20842, 19949, 19121, 18386, 17651, 16998,
			16349, 15800, 15247, 14783, 14321, 13859, 13535, 13081, 12702,
			12423, 12056, 11779, 11513, 11135, 10955, 10676, 10392, 10208,
			9928, 9747, 9564, 9369, 9193, 9017, 8822, 8639, 8458,
			8270, 8084, 7896, 7710, 7527, 7347, 7156, 6977, 6788,
			6607, 6422, 6236, 6054, 5867, 5684, 5495, 5305, 5128,
			4945, 4751, 4638, 4442, 4248, 4065, 3888, 3698, 3509,
			3326, 3139, 2957, 2775, 2586, 2405, 2216, 2037, 1846,
			1666, 1483, 1297, 1109, 927, 735, 554, 375, 201,
			128, 0
		};

		//imagemagick estimate
		double sum = qtables[0].sum + qtables[1].sum;
		double qvalue = qtables[0].im_qval + qtables[1].im_qval;
		for (int i=0; i < 100; i++) {
			if ((qvalue < hash[i]) && (sum < sums[i])) {
				continue;
			}
			if (((qvalue <= hash[i]) && (sum <= sums[i])) || (i >= 50)) {
				imagick_quality = i + 1;
			}
			break;
		}
	}

	quality_estimates.push_back(imagick_quality);
	quality_estimates.push_back(hf_quality);

	return num_qtables;
}

/*
	Lexicographically sorts an index for DCT Copy-Move detection
*/
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

/*
	Copy-Move detection using DCT.

	implementation adapted from "Detection of Copy-Move Forgery in Digital Images"
	by Jessica Fridrich, David Soukal, Jan Lukas
	http://www.ws.binghamton.edu/fridrich/research/copymove.pdf

	implementation adapted from Samuel Albrecht's GIMP plugin
	https://sites.google.com/site/elsamuko/forensics/clone-detection

	This function is different from the above resources:
	- Instead of quantizing by the modified JPEG table, this will instead compare
		the square submatrix of the DCT values, where the submatrix length is the
		"retain" parameter
	- The matches with the same shift-vector magnitude get painted in the same (random) color
*/
void copy_move_dct(Mat &src, Mat &dst, int retain = 4, double qcoeff = 1.0) {
	Mat grayscale;
	cvtColor( src, grayscale, CV_BGR2GRAY );
	grayscale.convertTo(grayscale, CV_32F);

	Mat rectBuffer = src.clone();

	int subm_limit = retain * retain;

	int blocksize = 16;
	int blocks_height = src.rows-blocksize+1;
	int blocks_width = src.cols-blocksize+1;
	int total_blocks = blocks_height * blocks_width;

	vector< Mat > blocks;
	blocks.reserve(total_blocks);

	Mat tmp;

	for(int y=0; y<blocks_height; y++) {
		for(int x=0; x<blocks_width; x++) {
			dct(grayscale(Rect(x,y,blocksize,blocksize)), tmp);
			tmp = tmp / qcoeff;
			tmp.convertTo(tmp, CV_8U);
			blocks.push_back(tmp(Rect(0,0,retain,retain)).clone());
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

	addWeighted(src, 0.2, rectBuffer, 0.8, 0, dst);
}
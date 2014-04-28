#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/foreach.hpp>
#include <chrono>

#include "structs.h"
#include "functions.hpp"

using namespace std;
using namespace cv;
using namespace boost::program_options;
using namespace boost::filesystem;
using boost::property_tree::ptree;

//globals for run_analysis function
string output_stem;
ptree root;
bool output, display, autolevels;

//run_analysis constants
enum analysis_type {A_ELA, A_LG, A_AVGDIST, A_HSV, A_LAB, A_LAB_FAST, A_COPY_MOVE_DCT};
string analysis_name[] = {
	"Error Level Analysis", "Luminance Gradient", "Average Distance",
	"HSV Histogram", "Lab Histogram", "Lab Histogram (fast)", "Copy Move Detection (DCT)"
};
string analysis_abbr[] = {"ela", "lg", "avgdist", "hsv", "lab", "lab_fast", "copymove"};

//run analysis on src image
void run_analysis(Mat &src, Mat &dst, analysis_type type, vector<double> params) {
	string output_filepath = output_stem + "_" + analysis_abbr[type]; //file name
	string title = analysis_name[type]; //display window title
	string ptree_element = analysis_abbr[type]; //json tree title

	bool apply_autolevels = autolevels && (type == A_ELA || type == A_LG || type == A_AVGDIST);
	if(apply_autolevels) {
		output_filepath += "_autolevels.png";
		ptree_element += "_autolevels";
	} else {
		output_filepath += ".png";
	}

	switch(type) {
		case A_ELA:
			error_level_analysis(src, dst, params[0]);
			root.put(ptree_element + ".quality", params[0]);
			break;
		case A_LG:
			luminance_gradient(src, dst);
			break;
		case A_AVGDIST:
			average_distance(src, dst);
			break;
		case A_HSV:
			hsv_histogram(src, dst, params[0]);
			root.put(ptree_element + ".whitebg", (bool)params[0]);
			break;
		case A_LAB:
			lab_histogram(src, dst, params[0]);
			root.put(ptree_element + ".whitebg", (bool)params[0]);
			break;
		case A_LAB_FAST:
			lab_histogram_fast(src, dst, params[0]);
			root.put(ptree_element + ".whitebg", (bool)params[0]);
			break;
		case A_COPY_MOVE_DCT:
			copy_move_dct(src, dst, params[0], params[1]);
			root.put(ptree_element + ".retain", params[0]);
			root.put(ptree_element + ".qcoeff", params[1]);
			break;
	}

	if(apply_autolevels) {
		hsv_histogram_stretch(dst, dst);
	}

	if(output) { //output image & add to ptree
		bool write_success = imwrite(output_filepath, dst);
		if(!write_success) {
			root.put(ptree_element + ".filename", "Error! Do you have write permission?");
		} else {
			string filepath = canonical(output_filepath).make_preferred().string();
			root.put(ptree_element + ".filename", filepath);
		}
	}
	
	if(display) { //display right away, waitKey(0) at the end of program
		namedWindow(title);
		imshow(title, dst);
	} else { //release memory
		dst.release();
	}
}

//override ostream << operator for vector<double> so we can use it as implicit_value
//or as boost puts it, make vector<double> ostream'able
namespace std {
	static std::ostream& operator<<(std::ostream& os, const std::vector<double>& v) {
		os << '{';
		for(int i=0; i<v.size(); i++) {
			if (i!=0)
			os << ", ";
			os << v[i];
		}
		os << '}';
		return os;
	}
}

int main(int argc, char *argv[]) {
	//declare program options
	options_description desc("USAGE: phoenix -f <path_to_file> [options]\nAllowed options");
	desc.add_options()
		("help,h", "List all arguments - produce help message")
		("file,f", value<string>()->required(), "Source image file")
		("output,o", value<string>()->implicit_value("./"), "Output folder path")

		("ela", value<int>()->implicit_value(70), "Error Level Analysis [optional resave quality]")
		("hsv", value<int>()->implicit_value(0), "HSV Colorspace Histogram")
		("lab", value<int>()->implicit_value(0), "Lab Colorspace Histogram")
		("labfast", value<int>()->implicit_value(0), "Lab Colorspace Histogram (Fast Version)")
		("lg", bool_switch()->default_value(false), "Luminance Gradient")
		("avgdist", bool_switch()->default_value(false), "Average Distance")
		("copymove", value<vector<double>>()->multitoken()->implicit_value(vector<double>{4, 1.0}), "Copy-Move Detection (DCT)")

		("autolevels,a", bool_switch()->default_value(false), "Apply histogram stretch (Auto-Levels) to outputs")
		("quality,q", bool_switch()->default_value(true), "Estimate JPEG Quality")

		("display,d", bool_switch()->default_value(false), "Display outputs")
		("verbose,v", bool_switch()->default_value(false), "Verbose (debug) mode")
		("json,j", bool_switch()->default_value(false), "Output JSON")
	;

	variables_map vm;

	try { //try to parse command options
		store(
			command_line_parser(argc, argv).options(desc)
			.style(
				command_line_style::allow_short
				| command_line_style::short_allow_next
				| command_line_style::short_allow_adjacent
				| command_line_style::allow_dash_for_short
				| command_line_style::allow_long
				| command_line_style::long_allow_next
				| command_line_style::long_allow_adjacent
				| command_line_style::allow_long_disguise
				).run()
			, vm);

		if (vm.count("help")) { //print help text before notify()
			cout << desc << endl;
			return 0;
		}
		notify(vm); //send commands to variables_map
	} catch (const exception &e) { //error with command options
		cout << "Error: Cannot parse program commands!" << endl;
		cout << e.what() << endl;
		cout << "Use -h or -help flag to see available commands." << endl;
		return 1;
	} catch(...) {
		cout << "Error: Fatal error while parsing options." << endl;
		return 1;
	}

	//some path info
	path source_path;
	path output_path;
	Mat source_image;

	try { //check and try to open source image file (-f)
		source_path = vm["file"].as<string>();
		if(!exists(source_path)) {
			cout << "Error: File not found!" << endl;
			cout << "File path input: " << source_path << endl;
			return 1;
		}

		//load image to memory
		source_image = imread(source_path.string(), CV_LOAD_IMAGE_COLOR);
		if(source_image.data == NULL) {
			cout << "Error: Cannot read image!" << endl;
			cout << "File path input: " << source_path << endl;
			return 1;
		}

		//validate output path
		if(vm.count("output")) {
			output_path = vm["output"].as<string>();
			if(!is_directory(output_path)) {
				cout << "Error: Output directory does not exist!" << endl;
				cout << "Output directory input: " << output_path << endl;
				return 1;
			}
			output_path = canonical(output_path.make_preferred());
		}
	} catch(const exception &e) { //cannot load the image for some reason
		cout << "Error: Problem while opening the file!" << endl;
		cout << e.what() << endl;
		return 1;
	}

	//assign globals
	display = vm["display"].as<bool>();
	output = vm.count("output");
	autolevels = vm["autolevels"].as<bool>();
	output_stem = output_path.string() + "/" + source_path.stem().string();
	
	bool verbose = vm["verbose"].as<bool>();

	if(vm.count("ela")) {
		Mat ela;
		vector<double> params {(double) vm["ela"].as<int>()};

		run_analysis(source_image, ela, A_ELA, params);
	}

	if(vm["lg"].as<bool>()) {
		Mat lg;
		vector<double> params;

		run_analysis(source_image, lg, A_LG, params);
	}

	if(vm["avgdist"].as<bool>()) {
		Mat avgdist;
		vector<double> params;

		run_analysis(source_image, avgdist, A_AVGDIST, params);
	}

	if(vm.count("hsv")) {
		Mat hsv;
		vector<double> params {(double) vm["hsv"].as<int>()};

		run_analysis(source_image, hsv, A_HSV, params);
	}

	if(vm.count("lab")) {
		Mat lab;
		vector<double> params {(double) vm["lab"].as<int>()};

		run_analysis(source_image, lab, A_LAB, params);
	}

	if(vm.count("labfast")) {
		Mat lab;
		vector<double> params {(double) vm["labfast"].as<int>()};

		run_analysis(source_image, lab, A_LAB_FAST, params);
	}

	if(vm.count("copymove")) {
		Mat copymove;
		vector<double> input = vm["copymove"].as<vector<double>>();
		vector<double> params;
		if(input.size() == 1) {
			if(input[0] > 16) input[0] = 16;
			params = {input[0], 1.0};
		} else {
			params = input;
		}

		run_analysis(source_image, copymove, A_COPY_MOVE_DCT, params);
	}

	if(vm["quality"].as<bool>()) {
		int num_qtables = 0;
		vector<qtable> qtables;
		vector<double> quality;

		num_qtables = estimate_jpeg_quality(source_path.string().c_str(), qtables, quality);

		if(num_qtables > 0) { //if we have quantization tables, save them to ptree
			root.put("imagick_estimate", quality[0]);
			root.put("hf_estimate", quality[1]);
			for(int i=0; i<num_qtables; i++) { //loop through the table and append as comma separated vals
				stringstream dqt;
				for(int j=0; j<8; j++) {
					for(int k=0; k<8; k++) {
						dqt << qtables[i].table.at<float>(j, k);
						if(j*k < 48) {
							dqt << ",";
						}
					}
				}
				stringstream tableindex;
				tableindex << "qtables." << i;
				root.put(tableindex.str(), dqt.str());
			}
		}
	}

	if(vm.count("output") == 0 && vm["display"].defaulted()) {
		cout << "Warning: No -output or -display option specified. You might want to use one (or both)." << endl;
	}

	if(vm["json"].as<bool>() || !vm["quality"].defaulted()) {
		write_json(cout, root);
	}

	if(display) {
		waitKey(0);
	}

	return 0;
}
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

#include <chrono>

#include "structs.h"
#include "functions.hpp"

using namespace std;
using namespace cv;
using namespace boost::program_options;
using namespace boost::filesystem;
using boost::property_tree::ptree;

//for function switcher and verbose mode
enum analysis_type {A_ELA, A_LG, A_AVGDIST, A_HSV, A_LAB, A_LAB_FAST};
string analysis_names[] = {"ELA", "LG", "AVGDIST", "HSV", "LAB", "LAB_FAST"};

//globals for run_analysis function
string output_stem;
ptree root;
bool output, display, verbose, autolevels;

//run analysis on src image
void run_analysis(Mat &src, Mat &dst, analysis_type type, vector<int> params) {
	if(verbose) {
		cout << "DEBUG: " << analysis_names[type] << " Starting..." << endl;
		system("pause");
	}

	string output_filepath; //for saving
	string title; //for display
	string ptree_element; //for json output

	switch(type) {
		case A_ELA:
			output_filepath = output_stem + "_ela";
			title = "Error Level Analysis";
			ptree_element = "ela";
			error_level_analysis(src, dst, params[0]);
			break;
		case A_LG:
			output_filepath = output_stem + "_lg";
			title = "Luminance Gradient";
			ptree_element = "lg";
			luminance_gradient(src, dst);
			break;
		case A_AVGDIST:
			output_filepath = output_stem + "_avgdist";
			title = "Average Distance";
			ptree_element = "avgdist";
			average_distance(src, dst);
			break;
		case A_HSV:
			output_filepath = output_stem + "_hsv";
			title = "HSV Histogram";
			ptree_element = "hsv";
			hsv_histogram(src, dst, params[0]);
			break;
		case A_LAB:
			output_filepath = output_stem + "_lab";
			title = "Lab Histogram";
			ptree_element = "lab";
			lab_histogram(src, dst);
			break;
		case A_LAB_FAST:
			output_filepath = output_stem + "_lab_fast";
			title = "Lab Histogram";
			ptree_element = "lab_fast";
			lab_histogram_fast(src, dst);
			break;
	}

	if(autolevels && type != A_HSV & type != A_LAB & type != A_LAB_FAST) {
		hsv_histogram_stretch(dst, dst);
		output_filepath += "_autolevels.png";
		ptree_element += "_autolevels";
	} else {
		output_filepath += ".png";
	}

	if(output) { //output image & add to ptree
		bool write_success = imwrite(output_filepath, dst);
		// if(!write_success) cout << "cant write" << endl;

		string filepath = canonical(output_filepath).make_preferred().string();
		root.put(ptree_element + ".filename", filepath);
	}
	
	if(display) { //display right away, waitKey(0) at the end of program
		namedWindow(title);
		imshow(title, dst);
	} else { //release memory
		dst.release();
	}

	if(verbose) {
		cout << "DEBUG: " << analysis_names[type] << " Finished." << endl;
		system("pause");
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
		// ("borders", bool_switch()->default_value(false), "Show RGB borders in histograms")
		("lg", bool_switch()->default_value(false), "Luminance Gradient")
		("avgdist", bool_switch()->default_value(false), "Average Distance")
		("quality,q", bool_switch()->default_value(true), "Estimate JPEG Quality")
		// ("dct", bool_switch()->default_value(false), "DCT")
		("display,d", bool_switch()->default_value(false), "Display outputs")
		("autolevels", bool_switch()->default_value(false), "Apply histogram stretching (Auto-Levels) to outputs")
		("invoke", value<string>(), "Invoke php script after execution")
		("verbose,v", bool_switch()->default_value(false), "Verbose (debug) mode")
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
		cout << desc << endl;
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
		if(!exists(vm["file"].as<string>())) {
			cout << "Error: File not found!" << endl;
			return 1;
		}

		//load image to memory
		source_image = imread(vm["file"].as<string>(), CV_LOAD_IMAGE_COLOR);
		if(source_image.data == NULL) {
			cout << "Error: Cannot read image!" << endl;
			return 1;
		}

		source_path = vm["file"].as<string>();
		if(vm.count("output") && !is_directory(vm["output"].as<string>())) {
			cout << "Error: Output directory does not exist!" << endl;
			return 1;
		}

		if(vm.count("output")) {
			output_path = vm["output"].as<string>();
			output_path = canonical(output_path.make_preferred());
		}
	} catch(const exception &e) { //cannot load the image for some reason
		cout << "Error: Problem while opening the file!" << endl;
		cout << e.what() << endl;
		return 1;
	}

	//assign globals
	display = vm["display"].as<bool>();
	verbose = vm["verbose"].as<bool>();
	output = vm.count("output");
	autolevels = vm["autolevels"].as<bool>();
	output_stem = output_path.string() + "/" + source_path.stem().string();

	if(verbose) {
		cout << "DEBUG: Image Loaded. Ready to go..." << endl;
		system("pause");
	}

	if(vm.count("ela")) {
		Mat ela;
		vector<int> params;
		params.push_back(vm["ela"].as<int>());

		run_analysis(source_image, ela, A_ELA, params);
	}

	if(vm["lg"].as<bool>()) {
		Mat lg;
		vector<int> params;

		run_analysis(source_image, lg, A_LG, params);
	}

	if(vm["avgdist"].as<bool>()) {
		Mat avgdist;
		vector<int> params;

		run_analysis(source_image, avgdist, A_AVGDIST, params);
	}

	if(vm.count("hsv")) {
		Mat hsv;
		vector<int> params;
		params.push_back(vm["hsv"].as<int>());

		run_analysis(source_image, hsv, A_HSV, params);
	}

	if(vm.count("lab")) {
		Mat lab;
		vector<int> params;
		params.push_back(vm["lab"].as<int>());

		run_analysis(source_image, lab, A_LAB, params);
	}

	if(vm.count("labfast")) {
		Mat lab;
		vector<int> params;
		params.push_back(vm["labfast"].as<int>());

		run_analysis(source_image, lab, A_LAB_FAST, params);
	}

	int num_qtables = 0;
	vector<qtable> qtables;
	vector<double> quality;

	if(vm["quality"].as<bool>()) {
		num_qtables = estimate_jpeg_quality(vm["file"].as<string>().c_str(), qtables, quality);
	}

	// if(vm["dct"].as<bool>()) {
	// 	dct_madness(source_image);
	// }

	if(vm.count("output")) {
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
		} else {
		}

		//invoke the -invoke param as php script, passing in the ptree as json
		if(vm.count("invoke")) {
			stringstream data;
			write_json(data, root, false);
			string json_string = data.str();
			boost::algorithm::replace_all(json_string, "\"", "\\\""); //escape quotes
			string script_call = "php " + vm["invoke"].as<string>() + " \"" + json_string + "\"";
			// cout << script_call << endl;
			cout << system(script_call.c_str()) << endl;
		} else { //output results to stdout
			// write_ini(cout, root);
			cout << "Last Resave Quality" << endl;
			cout << "  ImageMagick Estimate: " << (int) quality[0] << endl;
			cout << "  Hackerfactor Estimate: " << (int) quality[1] << endl;

			cout << "  Quantization Tables" << endl;
			for(int i=0; i<num_qtables; i++) {
				cout << "    " << (i == 0 ? "Luminance" : "Chrominance") << endl;
				for(int j=0; j<8; j++) {
					cout << "    ";
					for(int k=0; k<8; k++) {
						cout << qtables[0].table.at<float>(j, k) << " ";
					}
					cout << endl;
				}
			}
		}

		if(verbose) {
			write_json(cout, root);
		}
	}

	if(vm["display"].as<bool>()) {
		waitKey(0);
	}

	if(vm["verbose"].as<bool>()) {
		cout << "DEBUG: All done." << endl;
	}

	return 0;
}
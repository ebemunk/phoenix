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
#include "functions.h"

using namespace std;
using namespace cv;
using namespace boost::program_options;
using namespace boost::filesystem;
using boost::property_tree::ptree;

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
	    // ("borders", bool_switch()->default_value(false), "Show RGB borders in histograms")
	    ("lg", bool_switch()->default_value(false), "Luminance Gradient")
	    ("avgdist", bool_switch()->default_value(false), "Average Distance")
	    ("quality,q", bool_switch()->default_value(true), "Estimate JPEG Quality")
	    // ("dct", bool_switch()->default_value(false), "DCT")
	    ("display,d", bool_switch()->default_value(false), "Display outputs")
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
		notify(vm);
	} catch (const exception &e) {
		cout << "Erorz!" << endl;
		cout << e.what() << endl;
		return 1;
	} catch(...) {
		cout << "FAK" << endl;
		return 1;
	}

	path source_path;
	path output_path;
	Mat source_image;

	try { //check and try to open source image file (-f)
		if(!exists(vm["file"].as<string>())) {
			cout << "no file" << endl;
			return 1;
		}

		source_image = imread(vm["file"].as<string>(), CV_LOAD_IMAGE_COLOR);
		if(source_image.data == NULL) {
			cout << "image cant be read" << endl;
			return 1;
		}

		source_path = vm["file"].as<string>();
		if(vm.count("output") && !is_directory(vm["output"].as<string>())) {
			cout << "no dir" << endl;
			return 1;
		}

		if(vm.count("output")) {
			output_path = vm["output"].as<string>();
			output_path = canonical(output_path.make_preferred());
		}
	} catch(const exception &e) {
		cout << "EX: " << e.what() << endl;
		return 1;
	}

	if(vm["verbose"].as<bool>()) {
		cout << "DEBUG: Image Loaded. Ready to go..." << endl;
		system("pause");
	}

	ptree root;

	if(vm.count("ela")) {
		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: ELA Starting..." << endl;
			system("pause");
		}

		Mat ela;
		error_level_analysis(source_image, ela, vm["ela"].as<int>());
		if(vm.count("output")) {
			string output_filepath = output_path.string() + "/" + source_path.stem().string() + "_ela.png";
			imwrite(output_filepath, ela);

			string filepath = canonical(output_filepath).make_preferred().string();
			root.put("ela.filename", filepath);
		}
		
		if(vm["display"].as<bool>()) {
			namedWindow("Error Level Analysis");
			imshow("Error Level Analysis", ela);
		} else {
			ela.release();
		}

		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: ELA Finished." << endl;
			system("pause");
		}
	}

	if(vm["lg"].as<bool>()) {
		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: LG Starting..." << endl;
			system("pause");
		}

		Mat lg;
		luminance_gradient(source_image, lg);
		if(vm.count("output")) {
			string output_filepath = output_path.string() + "/" + source_path.stem().string() + "_lg.png";
			imwrite(output_filepath, lg);

			string filepath = canonical(output_filepath).make_preferred().string();
			root.put("lg.filename", filepath);
		}
		
		if(vm["display"].as<bool>()) {
			namedWindow("Luminance Gradient");
			imshow("Luminance Gradient", lg);
		} else {
			lg.release();
		}

		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: LG Finished." << endl;
			system("pause");
		}
	}

	if(vm["avgdist"].as<bool>()) {
		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: Avgdist Starting..." << endl;
			system("pause");
		}

		Mat avgdist;
		average_distance(source_image, avgdist);
		if(vm.count("output")) {
			string output_filepath = output_path.string() + "/" + source_path.stem().string() + "_avgdist.png";
			imwrite(output_filepath, avgdist);

			string filepath = canonical(output_filepath).make_preferred().string();
			root.put("avgdist.filename", filepath);
		}
		
		if(vm["display"].as<bool>()) {
			namedWindow("Average Distance");
			imshow("Average Distance", avgdist);
		} else {
			avgdist.release();
		}

		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: Avgdist Finished." << endl;
			system("pause");
		}
	}

	if(vm.count("hsv")) {
		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: HSV Starting..." << endl;
			system("pause");
		}

		Mat hsv;
		hsv_histogram(source_image, hsv, vm["hsv"].as<int>());

		if(vm.count("output")) {
			string output_filepath = output_path.string() + "/" + source_path.stem().string() + "_hsv.png";
			imwrite(output_filepath, hsv);

			string filepath = canonical(output_filepath).make_preferred().string();
			root.put("hsv.filename", filepath);
		}
		
		if(vm["display"].as<bool>()) {
			namedWindow("HSV Histrogram");
			imshow("HSV Histogram", hsv);
		} else {
			hsv.release();
		}

		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: HSV Finished." << endl;
			system("pause");
		}
	}

	if(vm.count("lab")) {
		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: Lab Starting..." << endl;
			system("pause");
		}

		Mat lab;
		lab_histogram_fast(source_image, lab, vm["lab"].as<int>());
		if(vm.count("output")) {
			string output_filepath = output_path.string() + "/" + source_path.stem().string() + "_lab.png";
			imwrite(output_filepath, lab);

			string filepath = canonical(output_filepath).make_preferred().string();
			root.put("lab.filename", filepath);
		}
		
		if(vm["display"].as<bool>()) {
			namedWindow("Lab Histogram");
			imshow("Lab Histogram", lab);
		} else {
			lab.release();
		}

		if(vm["verbose"].as<bool>()) {
			cout << "DEBUG: Lab Finished." << endl;
			system("pause");
		}
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
		if(num_qtables > 0) {
			root.put("imagick_estimate", quality[0]);
			root.put("hf_estimate", quality[1]);
			for(int i=0; i<num_qtables; i++) {
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
				tableindex << "dqt." << i;
				root.put(tableindex.str(), dqt.str());
			}
		}

		if(vm.count("invoke")) {
			//cout << vm["invoke"].as<string>() << endl;
			stringstream data;
			write_json(data, root, false);
			string json_string = data.str();
			boost::algorithm::replace_all(json_string, "\"", "\\\"");
			string script_call = "php " + vm["invoke"].as<string>() + " \"" + json_string + "\"";
			// cout << script_call << endl;
			cout << system(script_call.c_str()) << endl;
			//write_json(cout, root);
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
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>

#include "functions.h"

using namespace std;
using namespace cv;
using namespace boost::program_options;
using namespace boost::filesystem;
using boost::property_tree::ptree;

struct analysis {
	string type; //type of anaylsis
	Mat image; //analysis image
	string filename; //for saving
	string title; //for display
};

analysis make_analysis(string type, Mat image, string filename, string title) {
	analysis a = {type, image, filename, title};
	return a;
}

int main(int argc, char *argv[]) {
	/*cout << "HELO: " << endl;
	cout << system("php C:\\wamp\\www\\testmysql.php") << endl;
	cout << "AFTER: " << endl;
	return 0;*/
	//declare program options
	options_description desc("USAGE: phoenix -f <path_to_file> [options]\nAllowed options");
	desc.add_options()
	    ("help,h", "List all arguments - produce help message")
	    ("file,f", value<string>()->required(), "Source image file")
	    ("output,o", value<string>()->implicit_value("./"), "Output folder path")
	    ("ela", value<int>()->implicit_value(70), "Error Level Analysis [optional resave quality]")
	    ("hsv", value<int>()->implicit_value(0), "HSV Colorspace Histogram")
	    ("lab", value<int>()->implicit_value(0), "Lab Colorspace Histogram")
	    ("borders", bool_switch()->default_value(false), "Show RGB borders in histograms")
	    ("lg", bool_switch()->default_value(false), "Luminance Gradient")
	    ("avgdist", bool_switch()->default_value(false), "Average Distance")
	    ("quality,q", bool_switch()->default_value(true), "Estimate JPEG Quality")
	    ("dct", bool_switch()->default_value(false), "DCT")
	    ("display,d", bool_switch()->default_value(false), "Display outputs")
	    ("invoke", value<string>(), "Invoke php script after execution")
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
	vector<pair<Mat, string> > image_list;

	vector<analysis> analysis_list;

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

	if(vm.count("ela")) {
		Mat ela;
		error_level_analysis(source_image, ela, vm["ela"].as<int>());
		analysis_list.push_back(make_analysis("ela", ela, "ela.png", "Error Level Analysis"));
	}

	if(vm["lg"].as<bool>()) {
		Mat lg;
		luminance_gradient(source_image, lg);
		analysis_list.push_back(make_analysis("lg", lg, "lg.png", "Luminance Gradient"));
	}

	if(vm["avgdist"].as<bool>()) {
		Mat avgdist;
		average_distance(source_image, avgdist);
		analysis_list.push_back(make_analysis("avgdist", avgdist, "avgdist.png", "Average Distance"));
	}

	if(vm.count("hsv")) {
		Mat hsv;
		string hsv_filename;
		if(vm["borders"].as<bool>()) {
			Mat rgb;
			rgb_borders(rgb);
			hsv_histogram(rgb, hsv, vm["hsv"].as<int>());
		} else {
			hsv_histogram(source_image, hsv, vm["hsv"].as<int>());
			hsv_filename = "hsv.png";
		}
		analysis_list.push_back(make_analysis("hsv", hsv, hsv_filename, "HSV Histogram"));
	}

	if(vm.count("lab")) {
		Mat lab;
		string lab_filename;
		if(vm["borders"].as<bool>()) {
			Mat rgb;
			rgb_borders(rgb);
			lab_histogram(rgb, lab, vm["lab"].as<int>());
		} else {
			lab_histogram(source_image, lab, vm["lab"].as<int>());
			lab_filename = "lab.png";
		}
		analysis_list.push_back(make_analysis("lab", lab, lab_filename, "Lab Histogram"));
	}

	int num_qtables = 0;
	vector<qtable> qtables;
	vector<double> quality;

	if(vm["quality"].as<bool>()) {
		num_qtables = estimate_jpeg_quality(vm["file"].as<string>().c_str(), qtables, quality);
	}

	if(vm["dct"].as<bool>()) {
		dct_madness(source_image);
	}

	if(vm.count("output")) {
		ptree root;
		for(vector<analysis>::iterator it = analysis_list.begin(); it != analysis_list.end(); ++it) {
			string output_filepath = output_path.string() + "/" + source_path.stem().string() + "_" + it->filename;
			imwrite(output_filepath, it->image);
			root.put(it->type + string(".filename"), canonical(output_filepath).make_preferred().string());
		}

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
			cout << script_call << endl;
			/*cout << data.str() << endl;*/
			cout << system(script_call.c_str()) << endl;
			//cout << system();
			//write_json(cout, root);
		}
	}

	if(vm["display"].as<bool>()) {
		for(vector<analysis>::iterator it = analysis_list.begin(); it != analysis_list.end(); ++it) {
			namedWindow(it->title);
			imshow(it->title, it->image);
		}
		waitKey(0);
	}

	return 0;
}
#include <string>
#include <sstream>
#include <iostream>
#include <chrono>

#include "debugger.hpp"

using namespace std;

//set to inactive on creation
bool debugger::active = false;

//lazy static singleton
debugger& debugger::instance() {
	static debugger instance;

	return instance;
}

//start timing & print
void debugger::start(string msg) {
	if(!active) return;

	t_start = chrono::high_resolution_clock::now();
	print(msg + " starting");
}

//end timing & print
void debugger::end(string msg) {
	if(!active) return;

	t_end = chrono::high_resolution_clock::now();

	int secs = chrono::duration_cast<chrono::seconds>(t_end - t_start).count();
	int millisecs = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count();
	
	stringstream ss;
	ss << msg << " took: " << secs << ":" << millisecs;
	string str = ss.str();
	
	print(str);
}

//simple cout wrapper (for now)
void debugger::print(string msg) {
	if(!active) return;

	cout << "DEBUG: " << msg << endl;
}
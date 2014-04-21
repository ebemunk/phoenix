#include <string>
#include <chrono>
#include <iostream>

#include "debugger.hpp"

using namespace std;

debugger& debugger::instance() {
	static debugger instance;

	return instance;
}

void debugger::start(string msg) {
	t_start = chrono::high_resolution_clock::now();
	cout << endl << msg << " starting" << endl;
}

void debugger::end(string msg) {
	t_end = chrono::high_resolution_clock::now();

	int secs = chrono::duration_cast<chrono::seconds>(t_end - t_start).count();
	int millisecs = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count();
	cout << msg << " took: " << secs << ":" << millisecs << endl << endl;
}
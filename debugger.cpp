#include <string>
#include <chrono>
#include <iostream>

#include "debugger.hpp"

using namespace std;

bool debugger::active = false;

debugger& debugger::instance() {
	static debugger instance;

	return instance;
}

void debugger::start(string msg) {
	if(!active) return;

	t_start = chrono::high_resolution_clock::now();
	cout << endl << msg << " starting" << endl;
}

void debugger::end(string msg) {
	if(!active) return;

	t_end = chrono::high_resolution_clock::now();

	int secs = chrono::duration_cast<chrono::seconds>(t_end - t_start).count();
	int millisecs = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count();
	cout << msg << " took: " << secs << ":" << millisecs << endl << endl;
}
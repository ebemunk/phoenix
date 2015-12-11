#include <string>
#include <chrono>

using namespace std;

/*
	Singleton debugger for timing stuff and debug messages
*/
class debugger {
	private:
		chrono::high_resolution_clock::time_point t_start, t_end;

		debugger() {}

		debugger(debugger const&);
		void operator=(debugger const&);

	public:
		static debugger& instance();
		static bool active;

		void start(string msg);
		void end(string msg);
		void print(string msg);
};
#include <string>
#include <chrono>

using namespace std;

class debugger {
	private:
		chrono::system_clock::time_point t_start, t_end;

		debugger() {}

		debugger(debugger const&);
		void operator=(debugger const&);

	public:
		static debugger& instance();

		void start(string msg);
		void end(string msg);
};
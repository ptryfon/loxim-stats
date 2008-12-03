#include <Util/Backtrace.h>
#include <Util/smartptr.h>
#include <string>
#include <execinfo.h>

using namespace std;
using namespace _smptr;

vector<string> Util::get_backtrace(int omit)
{
	void *array[30];
	vector<string> bt;
	size_t size = backtrace(array, 30);
	auto_ptr<char *> strings(backtrace_symbols(array, size));

	for (size_t i = 2; i < size; ++i){
		bt.push_back(strings.get()[i]);
	}
	return bt;
}


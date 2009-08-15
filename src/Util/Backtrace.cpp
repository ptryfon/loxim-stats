#include <Util/Backtrace.h>
#include <Util/smartptr.h>
#include <string>
#include <config.h>
#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

using namespace std;
using namespace _smptr;

vector<string> Util::get_backtrace(int /* omit */)
{
	vector<string> bt;
	#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE)
	void *array[30];
	size_t size = backtrace(array, 30);
	auto_ptr<char *> strings(backtrace_symbols(array, size));

	for (size_t i = 2; i < size; ++i){
		bt.push_back(strings.get()[i]);
	}
	#else
	bt.push_back("No backtrace available.");
	#endif
	return bt;
}


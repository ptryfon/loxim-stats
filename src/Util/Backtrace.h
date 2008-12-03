#ifndef BACKTRACE_H
#define BACKTRACE_H

#include <vector>
#include <string>

namespace Util{
	std::vector<std::string> get_backtrace(int omit = 0);
}

#endif

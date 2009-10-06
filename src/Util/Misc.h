#ifndef _UTIL_MISC_H
#define _UTIL_MISC_H

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <Util/Backtrace.h>

#ifdef __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define __assertion_impl(x, y, z) \
	{if (!(x)) { \
		std::cerr << z << " assertion failed (" << TOSTRING(x) << ")" << \
		std::endl << "in function " << __PRETTY_FUNCTION__ << " (" << \
		__FILE__ << ":" << __LINE__ << "): " << std::endl << "Message: " << \
		y << std::endl << Util::string_backtrace(0); ::abort(); \
	} }

#ifndef NDEBUG
#define d_assert(x, y) __assertion_impl(x, y, "Debug")
#else
#define d_assert(x, y) { if (false && (x)); }
#endif

#define r_assert(x, y) __assertion_impl(x, y, "Release")

namespace Util {

	//this is not inteded to be used anywhere beyond this file
	inline std::string string_backtrace(int omit)
	{
		std::vector<std::string> bt(get_backtrace(omit));
		std::stringstream ss;
		for (std::vector<std::string>::const_iterator i = bt.begin(); i != bt.end(); ++i)
		{
			ss << *i << std::endl;
		}
		return ss.str();
	}

	
	class NonCopyable {
		public:
			NonCopyable();

		private:
			NonCopyable(const NonCopyable &);
	};

	class NonAssignable {
		private:
			NonAssignable &operator=(const NonAssignable &);
	};

	class NonValue : private NonCopyable, private NonAssignable {
	};

	//implementation

	inline NonCopyable::NonCopyable()
	{
	}

}

#endif /* _UTIL_MISC_H */

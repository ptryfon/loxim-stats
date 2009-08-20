#ifndef _UTIL_MISC_H
#define _UTIL_MISC_H

//TODO: make them work properly

#ifdef __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


namespace Util {
	
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

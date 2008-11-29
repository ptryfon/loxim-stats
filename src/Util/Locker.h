#ifndef LOCKER_H
#define LOCKER_H

#include <pthread.h>

namespace Util{
	class Locker{

		private:
			::pthread_mutex_t &mutex;

		public:
			Locker(::pthread_mutex_t &mutex);
			~Locker();
			
			void wait(::pthread_cond_t &cond);
	};
}

#endif

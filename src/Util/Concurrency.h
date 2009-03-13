#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <signal.h>
#include <pthread.h>

namespace Util{

	class Locker;
	class Unlocker;
	class CondVar;

	class Mutex{
		friend class Locker;
		friend class Unlocker;
		friend class CondVar;

		private:
			pthread_mutex_t mutex;
		public:
			Mutex();
			~Mutex();
			void lock();
			void unlock();
		private:
			
			//just to forbid value semantics
			Mutex(const Mutex &);
			Mutex &operator=(const Mutex &);
			
			/*lock/unlock methods are private and this is on
			 * purpose, use Locker/Unlocker classes*/

	};

	class CondVar{
		friend class Locker;
		
		public:
			CondVar();
			~CondVar();
			void signal();
			void wait(Mutex &m);
		private:
			/* wait is private on purpose - it should only be
			 * called with a mutex locked, so it's moved to Locker
			 * */
			pthread_cond_t cond;
			bool timed_wait(Mutex &m, struct timespec &tout);

	};

	class Masker{
		private:
			sigset_t old_mask;
		public:
			Masker(const sigset_t &);
			const sigset_t &get_old_mask();
			~Masker();
	};

	class Locker{

		private:
			Mutex &mutex;

		public:
			Locker(Mutex &mutex);
			~Locker();
			
			void wait(CondVar &cond);
			/* true if we were signalled, false if timeout occured */
			bool timed_wait(CondVar &cond, struct timespec &tout);
	};
	
	class Unlocker {

		private:
			Mutex &mutex;

		public:
			Unlocker(Mutex &mutex);
			~Unlocker();
			
	};

}

#endif /* CONCURRENCY_H */

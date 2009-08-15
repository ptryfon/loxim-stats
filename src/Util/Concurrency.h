#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <cassert>
#include <signal.h>
#include <pthread.h>

#include <Util/Misc.h>

//TODO: a temporary hack, because I can't figure out what's going on
//there
namespace Indexes {
	class IndexHandler;
}

namespace Util{

	class CondVar;

	template <class T>
	class _Unlocker;

	template <class T>
	class _Locker : private NonValue {

		protected:
			T &mutex;

		public:
			explicit _Locker(T &mutex);
			~_Locker();

			friend class _Unlocker<T>;
	};

	template <class T>
	class _Unlocker : private NonValue {

		private:
			_Locker<T> &locker;

		public:
			_Unlocker(_Locker<T> &locker);
			~_Unlocker();

	};


	class Mutex : private NonValue {
		friend class _Locker<Mutex>;
		friend class _Unlocker<Mutex>;
		friend class CondVar;

		//TODO: a temporary hack, because I can't figure out what's going on
		//there
		friend class Indexes::IndexHandler;

		private:
			void init() const;
			virtual void set_attr(pthread_mutexattr_t &) const;
		protected:
			mutable pthread_mutex_t mutex;
			mutable bool initialized;
		public:
			Mutex();
			virtual ~Mutex();
		private:
			
			/*lock/unlock methods are private and this is on
			 * purpose, use Locker/Unlocker classes*/
			void lock();
			void unlock();

		public:

			class MutexLocker : public _Locker<Mutex>
			{
				public:	
					explicit MutexLocker(Mutex &m);

					void wait(CondVar &cond);
					/* true if we were signalled, false if timeout occured */
					bool timed_wait(CondVar &cond, struct timespec &tout);

			};

			typedef MutexLocker Locker;
			typedef _Unlocker<Mutex> Unlocker;

	};

	class RecursiveMutex : public Mutex {
		private:
			virtual void set_attr(pthread_mutexattr_t &) const;
		public:
			typedef _Locker<RecursiveMutex> Locker;
			typedef _Unlocker<RecursiveMutex> Unlocker;
	};

	class CondVar : private NonValue {
		friend class Mutex::MutexLocker;
		
		public:
			CondVar();
			~CondVar();
			void signal();
			void broadcast();
		private:
			/* wait is private on purpose - it should only be
			 * called with a mutex locked, so it's moved to Locker
			 * */
			pthread_cond_t cond;
			bool timed_wait(Mutex &m, struct timespec &tout);
			void wait(Mutex &m);

	};

	class Masker : private NonValue {
		private:
			sigset_t old_mask;
		public:
			explicit Masker(const sigset_t &);
			const sigset_t &get_old_mask();
			~Masker();
	};


	class Semaphore : NonValue
	{

		public:
			enum LockMode {
				Read,
				Write,
				None,
				Upgrade
			};

			virtual ~Semaphore();

			virtual	void lock_write() = 0;
			virtual void unlock()	 = 0;
	};



	class RWSemaphore : public Semaphore
	{
		private:
			Mutex mutex;
			CondVar reader_cond;
			CondVar writer_cond;

			LockMode current_mode;
			int wait_writers;
			int wait_readers;

			int inside;		
		public:
			RWSemaphore();

			virtual void lock_read();
			virtual void lock_write();
			virtual void unlock();
	};


	class RWUJSemaphore: public RWSemaphore
	{
		private:
			Mutex mutex;
			CondVar reader_cond;
			CondVar writer_cond;
			CondVar upgrader_cond;

			LockMode current_mode;
			int wait_writers;
			int wait_readers;
			int wait_upgraders;
			int best_upgrader;	/* an arbitrary id, eg. TransactionID */

			int inside;
			void _unlock(Mutex::Locker &);
			void _lock_write(Mutex::Locker &);
		public:
			RWUJSemaphore();

			virtual void lock_read();
			virtual void lock_write();
			virtual bool try_lock_write();
			virtual void unlock();
			virtual bool lock_upgrade(int id);
	};

	template <class T>
	inline _Locker<T>::_Locker(T &mutex) : mutex(mutex)
	{
		mutex.lock();
	}


	template <class T>
	inline _Locker<T>::~_Locker()
	{
		try {
			mutex.unlock();
		} catch (...) {
			assert(false);
			//do some reporting
		}
	}

	template <class T>
	inline _Unlocker<T>::_Unlocker(_Locker<T> &locker) : locker(locker)
	{
		locker.mutex.unlock();
	}


	template <class T>
	inline _Unlocker<T>::~_Unlocker()
	{
		try {
			locker.mutex.lock();
		} catch (...) {
			assert(false);
			//do some reporting
		}
	}

	
	inline void Mutex::MutexLocker::wait(CondVar &cond)
	{	
		cond.wait(mutex);
	}
	
	inline bool Mutex::MutexLocker::timed_wait(CondVar &cond, struct timespec &t)
	{	
		return cond.timed_wait(mutex, t);
	}
	
	inline Mutex::MutexLocker::MutexLocker(Mutex &mutex) : _Locker<Mutex>(mutex)
	{
	}


}

#endif /* CONCURRENCY_H */

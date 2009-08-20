#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <cassert>
#include <cstring>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#include <Util/Misc.h>


#ifndef DONT_DETECT_TAS_IMPL
#if defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
	#define TAS_IMPL_GNU_BUILTIN

#elif  defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

	#ifdef __i386__
	#define TAS_IMPL_ASM_i386
	#else
	#define TAS_IMPL_ASM_amd64
	#endif
#else
#warning Your compiler is not 'gcc-4.1 or newer' and your architecture is not\
	i386 neither amd64,	so I don't know how to provide the 'test and set'\
	operation. There is an option to use a fallback implementation using\
	pthreads, but it will be much much less effective. If you're still\
	comfortable with that, you can disable this warning. Patches for\
	different architectures or compilers will be appreciated.

	#define TAS_IMPL_PTHREADS
#endif
#endif /* DONT_DETECT_TAS_IMPL */


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

#ifdef TAS_IMPL_GNU_BUILTIN

#define SPIN_IMPL int
#define SPIN_IMPL_LOCKED 187
#define SPIN_IMPL_UNLOCKED 245
#define SPIN_IMPL_INITIALIZER SPIN_IMPL_UNLOCKED

	static inline void SPIN_IMPL_LOCK(SPIN_IMPL *lock) 
	{
		while (unlikely(!__sync_bool_compare_and_swap(lock, SPIN_IMPL_UNLOCKED, SPIN_IMPL_LOCKED))) {}
	}

	static inline void SPIN_IMPL_UNLOCK(SPIN_IMPL *lock)
	{
		bool res = __sync_bool_compare_and_swap(lock, SPIN_IMPL_LOCKED, SPIN_IMPL_UNLOCKED);
		res = res; //hack for poor assert implementation.
		assert(res);
	}

	static inline void SPIN_IMPL_INIT(SPIN_IMPL *lock)
	{
		*lock = SPIN_IMPL_UNLOCKED;
	}

	static inline bool SPIN_IMPL_TRYLOCK(SPIN_IMPL *lock)
	{
		return __sync_bool_compare_and_swap(lock, SPIN_IMPL_UNLOCKED, SPIN_IMPL_LOCKED);
	}

#elif defined(TAS_IMPL_ASM_i386) || defined(TAS_IMPL_ASM_amd64)

#define SPIN_IMPL int
#define SPIN_IMPL_LOCKED 187
#define SPIN_IMPL_UNLOCKED 245
#define SPIN_IMPL_INITIALIZER SPIN_IMPL_UNLOCKED

#define EXCHANGE(lock, locker) {\
	asm volatile ("xchgl %1, (%0);" \
			:"=r"(lock), "=r"(locker) \
			:"0"(lock), "1"(locker) \
			);\
}

	static inline void SPIN_IMPL_LOCK(SPIN_IMPL *lock) 
	{
		volatile SPIN_IMPL locker = SPIN_IMPL_LOCKED;
		do {
			EXCHANGE(lock, locker);
		} while (unlikely(locker == SPIN_IMPL_LOCKED));
		assert(locker == SPIN_IMPL_UNLOCKED);
	}

	static inline void SPIN_IMPL_UNLOCK(SPIN_IMPL *lock)
	{
		volatile SPIN_IMPL unlocker = SPIN_IMPL_UNLOCKED;
		EXCHANGE(lock, unlocker);
	}

	static inline void SPIN_IMPL_INIT(SPIN_IMPL *lock)
	{
		*lock = SPIN_IMPL_UNLOCKED;
	}

	static inline bool SPIN_IMPL_TRYLOCK(SPIN_IMPL *lock)
	{
		volatile SPIN_IMPL locker = SPIN_IMPL_LOCKED;
		EXCHANGE(lock, locker);
		return locker == SPIN_IMPL_UNLOCED;
	}

#elif defined(TAS_IMPL_PTHREADS)

#define SPIN_IMPL ::pthread_mutex_t
#define SPIN_IMPL_UNLOCKED PTHREAD_MUTEX_INITIALIZER
#define SPIN_IMPL_INITIALIZER SPIN_IMPL_UNLOCKED

	static inline void SPIN_IMPL_LOCK(SPIN_IMPL *lock)
	{
		int err;
		while ((err = pthread_mutex_trylock(lock)) == EBUSY) {}
		assert(err == 0);
	}

	static inline void SPIN_IMPL_UNLOCK(SPIN_IMPL *lock)
	{
		int err = pthread_mutex_unlock(lock);
		assert(err == 0);
	}

	static inline void SPIN_IMPL_INIT(SPIN_IMPL *lock)
	{
		pthread_mutex_init(lock, NULL);
	}

	static inline bool SPIN_IMPL_TRYLOCK(SPIN_IMPL *lock)
	{
		int err = pthread_mutex_trylock(lock);
		switch (err) {
			case EBUSY:
				return false;
			case 0:
				return true;
			default:
				assert(false);
		}
		//not reached
		return false;
	}
#else

#error Unknown 'test and set' implementation. This is probably a bug in the\
	implementation. Please report it. Include your operating system version,\
compiler and architecture (uname -m) or, even better, send us a patch.\
Thanks.

#endif

	class SpinLock : private NonValue {
		public:
			friend class _Locker<SpinLock>;

			typedef _Locker<SpinLock> Locker;

			SpinLock() 
			{
				SPIN_IMPL_INIT(&lock_impl);
			}


		private:
			SPIN_IMPL lock_impl;
			
			inline void lock()
			{
				SPIN_IMPL_LOCK(&(this->lock_impl));
			}

			inline void unlock()
			{
				SPIN_IMPL_UNLOCK(&(this->lock_impl));
			}

			inline bool try_lock()
			{
				return SPIN_IMPL_TRYLOCK(&(this->lock_impl));
			}

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



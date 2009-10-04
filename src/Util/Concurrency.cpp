#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <sstream>
#include <Util/Concurrency.h>
#include <Errors/Exceptions.h>

using namespace std;
using namespace Errors;

namespace Util{

	Mutex::Mutex() : initialized(false)
	{
	}

	Mutex::~Mutex()
	{
		int res = pthread_mutex_destroy(&mutex);
		r_assert(res == 0, "Destroying mutex failed");
	}

	void Mutex::set_attr(pthread_mutexattr_t &attr) const
	{
		int err;
		if ((err = pthread_mutexattr_init(&attr)))
			throw LoximException(err);
		if ((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL)))
			throw LoximException(err);
	}

	void Mutex::init() const
	{
		int err;
		pthread_mutexattr_t attr;
		set_attr(attr);
		if ((err = pthread_mutex_init(&mutex, &attr)))
			throw LoximException(err);
		initialized = true;
	}

	void Mutex::lock()
	{
		if (unlikely(!initialized))
			init();
		int error = pthread_mutex_lock(&mutex);
		if (error)
			throw LoximException(error);
	}
	
	void Mutex::unlock()
	{
		assert(initialized);
		int error = pthread_mutex_unlock(&mutex);
		if (error)
			throw LoximException(error);
	}

	void RecursiveMutex::set_attr(pthread_mutexattr_t & attr) const
	{
		int err;
		if ((err = pthread_mutexattr_init(&attr)))
			throw LoximException(err);
		if ((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)))
			throw LoximException(err);
	}

	CondVar::CondVar()
	{
		int error = pthread_cond_init(&cond, 0);
		if (error)
			throw LoximException(error);
	}
	
	CondVar::~CondVar()
	{
		pthread_cond_destroy(&cond);
		//XXX report failures
	}

	void CondVar::signal()
	{
		int error = pthread_cond_signal(&cond);
		if (error)
			throw LoximException(error);
	}
	
	void CondVar::broadcast()
	{
		int error = pthread_cond_broadcast(&cond);
		if (error)
			throw LoximException(error);
	}

	void CondVar::wait(Mutex &m)
	{
		int error = pthread_cond_wait(&cond, &(m.mutex));
		if (error)
			throw LoximException(error);
	}
	
	bool CondVar::timed_wait(Mutex &m, struct timespec &t)
	{
		int error = pthread_cond_timedwait(&cond, &(m.mutex), &t);
		if (error == ETIMEDOUT)
			return false;
		if (error)
			throw LoximException(error);
		return true;
	}

	Masker::Masker(const sigset_t &mask)
	{
		pthread_sigmask(SIG_SETMASK, &mask, &old_mask);
	}

	Masker::~Masker()
	{
		pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
	}

	const sigset_t &Masker::get_old_mask()
	{
		return old_mask;
	}



	Semaphore::~Semaphore()
	{
	}


	RWSemaphore::RWSemaphore() :
		writers_waiting(0),
		readers_waiting(0),
		inside(0),
		mode(None)
	{
	}


	void RWSemaphore::lock_read()
	{
		Mutex::Locker l(mutex);
		
		assert(mode != None || inside == 0); // mode == None --> inside == 0
		if (likely(mode == None || (mode == Read && writers_waiting == 0))) {
			assert(writers_waiting == 0);
			assert(inside == 0 || mode == Read);
			assert(readers_waiting == 0);
			assert(mode == None || mode == Read);
			mode = Read;
			assert(inside < static_cast<counters_type>(-1));
			++inside;
		} else {
			assert(mode == Write || writers_waiting != 0);
			assert(readers_waiting < static_cast<counters_type>(-1));
			++readers_waiting;
			try {
				l.wait(reader_cond);
			} catch (...) {
				--readers_waiting;
				throw;
			}
			assert(inside > 0);
			assert(mode == Read);
		}
	}
	
	void RWSemaphore::lock_write()
	{
		Mutex::Locker l(mutex);
		
		assert(mode != None || inside == 0); // mode == None --> inside == 0
		if (likely(mode == None)) {
			assert(inside == 0);
			assert(readers_waiting == 0 && writers_waiting == 0);
			mode = Write;
			inside = 1;
		} else {
				++writers_waiting;
			try {
				l.wait(writer_cond);
			} catch (...) {
				--writers_waiting;
				throw;
			}
			assert(inside == 1 && mode == Write);
		}
	}

	void RWSemaphore::unlock()
	{
		Mutex::Locker l(mutex);
		assert(inside > 0 && (mode == Read || mode == Write));
		inside--;
		if (likely(inside == 0)) {
			if (readers_waiting > 0) {
				if (mode == Read) {
					assert(writers_waiting > 0);
					mode = Write;
					--writers_waiting;
					inside = 1;
					try {
						writer_cond.signal();
					} catch (...) {
						assert(inside == 1);
						++writers_waiting;
						mode = Read;
						throw;
					}
				} else {
					assert(mode == Write);
					mode = Read;
					counters_type old_readers_waiting = readers_waiting;
					inside = readers_waiting;
					readers_waiting = 0;
					try {
						reader_cond.broadcast();
					} catch (...) {
						inside = 1;
						mode = Write;
						readers_waiting = old_readers_waiting;
						throw;
					}
				}
			} else if (writers_waiting > 0) {
				assert(readers_waiting == 0);
				LockMode old_mode = mode;
				mode = Write;
				--writers_waiting;
				inside = 1;
				try {
					writer_cond.signal();
				} catch (...) {
					assert(inside == 1);
					mode = old_mode;
					++writers_waiting;
					throw;
				}
			} else {
				assert(readers_waiting == 0 && writers_waiting == 0);
				mode = None;
			}
		} else {
			assert(mode == Read);
		}
	}

	RWUJSemaphore::RWUJSemaphore() :
		RWSemaphore(),
		upgrader_waiting(false),
		best_upgrader(NO_UPGRADER)
	{
	}

	void RWUJSemaphore::lock_read()
	{
		Mutex::Locker l(mutex);
		
		assert(mode != None || inside == 0); // mode == None --> inside == 0
		if (likely(mode == None || (mode == Read && writers_waiting == 0 && !upgrader_waiting))) {
			assert(best_upgrader == NO_UPGRADER);
			assert(writers_waiting == 0);
			assert(!upgrader_waiting);
			assert(inside == 0 || mode == Read);
			assert(readers_waiting == 0);
			assert(mode == None || mode == Read);
			mode = Read;
			assert(inside < static_cast<counters_type>(-1));
			++inside;
		} else {
			assert(mode == Write || writers_waiting > 0 || upgrader_waiting);
			assert(upgrader_waiting || best_upgrader == NO_UPGRADER);
			assert(readers_waiting < static_cast<counters_type>(-1));
			++readers_waiting;
			try {
				l.wait(reader_cond);
			} catch (...) {
				--readers_waiting;
				throw;
			}
			assert(inside > 0);
			assert(mode == Read);
		}
	}
	
	void RWUJSemaphore::lock_write()
	{
		Mutex::Locker l(mutex);
		
		assert(mode != None || inside == 0); // mode == None --> inside == 0
		if (likely(mode == None)) {
			assert(inside == 0);
			assert(readers_waiting == 0 && writers_waiting == 0 && !upgrader_waiting);
			assert(best_upgrader == NO_UPGRADER);
			mode = Write;
			inside = 1;
		} else {
				++writers_waiting;
			try {
				l.wait(writer_cond);
			} catch (...) {
				--writers_waiting;
				throw;
			}
			assert(inside == 1 && mode == Write);
		}
	}

	bool RWUJSemaphore::lock_upgrade(priority_type prio)
	{
		Mutex::Locker l(mutex);

		assert(mode != None || inside == 0); // mode == None --> inside == 0
		if (likely(mode == Read && inside == 1)) {
			mode = Write;
			upgrader_waiting = false;
			best_upgrader =  NO_UPGRADER;
			return true;
		} else {
			if (upgrader_waiting) {
				assert(best_upgrader != NO_UPGRADER);
				assert(best_upgrader != prio);
				if (best_upgrader > prio) {
					/*XXX how to make it exception safe? */
					upgrader_cond.signal();
					best_upgrader = prio;
					l.wait(upgrader_cond);
				} else {
					_unlock(l);
					return false;
				}
			} else {
				assert(best_upgrader == NO_UPGRADER);
				upgrader_waiting = true;
				best_upgrader = prio;
				try {
					l.wait(upgrader_cond);
				} catch (...) {
					upgrader_waiting = false;
					best_upgrader = NO_UPGRADER;
					throw;
				}
			}
			assert(upgrader_waiting);
			if (best_upgrader == prio) {
				assert(mode == Write);
				assert(inside == 1);
				upgrader_waiting = false;
				best_upgrader = NO_UPGRADER;
				return true;
			} else {
				assert(best_upgrader != NO_UPGRADER);
				_unlock(l);
				return false;
			}
		}
	}

	//BIG FAT WARNING!!!
	//If you're modifying this code, be very very careful about exception
	//safety. The code should be exception safe now. Keep in mind that broadcast
	//and signal methods may throw. Make sure (in most cases the order of
	//instructions is enough) that all attributes are in the same state as
	//before calling this method in case an exception is thrown. If you modify
	//anything, post to the mailing list and ask for code review.
	void RWUJSemaphore::_unlock(Mutex::Locker & l)
	{
		assert(inside > 0 && (mode == Read || mode == Write));
		inside--;
		if (likely(inside == 0)) {
			if (readers_waiting > 0) {
				if (mode == Read) {
					assert(writers_waiting > 0 || upgrader_waiting);
					inside = 1;
					writer_cond.signal();
					--writers_waiting;
					mode = Write;
				} else {
					assert(mode == Write);
					inside = readers_waiting;
					try {
						reader_cond.broadcast();
					} catch (...) {
						inside = 1;
						throw;
					}
					mode = Read;
					readers_waiting = 0;
				}
			} else if (writers_waiting > 0) {
				assert(readers_waiting == 0);
				inside = 1;
				writer_cond.signal();
				mode = Write;
				--writers_waiting;
			} else {
				assert(readers_waiting == 0 && writers_waiting == 0);
				mode = None;
			}
		} else if (likely(inside == 1)) {
			assert(mode == Read);
			if (upgrader_waiting) {
				assert(best_upgrader != NO_UPGRADER);
				upgrader_cond.signal();
				mode = Write;
			}
		} else {
			assert(mode == Read);
		}
	}

	void RWUJSemaphore::unlock()
	{
		Mutex::Locker l(mutex);
		_unlock(l);
	}

	bool RWUJSemaphore::try_lock_write()
	{
		assert(mode != None || inside == 0); // mode == None --> inside == 0
		if (likely(mode == None)) {
			assert(inside == 0);
			assert(readers_waiting == 0 && writers_waiting == 0 && !upgrader_waiting);
			assert(best_upgrader == NO_UPGRADER);
			mode = Write;
			inside = 1;
			return true;
		}
		return false;
	}
}


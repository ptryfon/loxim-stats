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
		pthread_mutex_destroy(&mutex);
		//XXX log it somewhere if it fails
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

//TODO: (CRITICAL) exception safety needs fixing in everything below


	void RWSemaphore::lock_read()
	{
		Mutex::Locker l(mutex);
		
		assert(mode != None || inside == 0); // inside == 0 --> mode == None
		if (likely(mode == None || (mode == Read && writers_waiting == 0 && readers_waiting == 0))) {
			//inside == 0 --> (readers_waiting == 0 && writers_waiting == 0)
			assert((readers_waiting != 0 || writers_waiting != 0) || (inside == 0 || mode == Read));
			assert(mode == None || mode == Read);
			mode = Read;
			assert(inside < static_cast<counters_type>(-1));
			++inside;
		} else {
			assert(mode == Write || writers_waiting != 0);
			assert(readers_waiting < static_cast<counters_type>(-1));
			++readers_waiting;
			l.wait(reader_cond);
			assert(mode == Read);
		}
	}
	
	void RWSemaphore::lock_write()
	{
		Mutex::Locker l(mutex);
		
		if (likely(mode == None)) {
			assert(inside == 0);
			assert(readers_waiting == 0 && writers_waiting == 0);
			mode = Write;
			inside = 1;
		} else {
			++writers_waiting;
			l.wait(writer_cond);
			assert(inside == 0 && mode == Write);
			assert(writers_waiting > 0);
			--writers_waiting;
			inside = 1;
		}
	}

	void RWSemaphore::unlock()
	{
		Mutex::Locker l(mutex);
		assert(inside > 0 && (mode == Read || mode == Write));
		inside--;
		if (likely(inside == 0)) {
			if (readers_waiting != 0) {
				if (mode == Read) {
					assert(writers_waiting > 0);
					mode = Write;
					writer_cond.signal();
				} else {
					assert(mode == Write);
					mode = Read;
					inside = readers_waiting;
					readers_waiting = 0;
					reader_cond.broadcast();
				}
			} else if (writers_waiting > 0) {
				assert(readers_waiting == 0);
				mode = Write;
				writer_cond.signal();
			} else {
				assert(readers_waiting == 0 && writers_waiting == 0);
				mode = None;
			}
		} else {
			assert(mode == Read);
		}
	}

	RWUJSemaphore::RWUJSemaphore() :
		current_mode(None),
		wait_writers(0),
		wait_readers(0),
		wait_upgraders(0),
		best_upgrader(-1),
		inside(0)
	{
	}

	bool RWUJSemaphore::try_lock_write() {
		Mutex::Locker l(mutex);
		if (current_mode != None)
			return false;
		else {
			_lock_write(l);
			return true;
		}
	}
	
	void RWUJSemaphore::lock_read()
	{
		Mutex::Locker l(mutex);
		
		if (current_mode == None)
			current_mode = Read;

		if (current_mode == Read && wait_writers == 0 && wait_upgraders == 0)
			/* reader can read - neither writer inside nor waiting */
			inside++;
		else {
			/* writer or updater is waiting or is inside - reader must wait */
			wait_readers++;
			l.wait(reader_cond);
			/* after signal re-aquire mutex */
			wait_readers--;
			inside++;
		}
	}
	
	void RWUJSemaphore::lock_write()
	{
		Mutex::Locker l(mutex);
		_lock_write(l);
	}

	void RWUJSemaphore::_lock_write(Mutex::Locker &l) {
		if (current_mode == None)
			current_mode = Write;

		if (inside == 0)
			inside++;
		else {
			wait_writers++;
			l.wait(writer_cond);
			/* after signal re-aquire mutex */
			wait_writers--;
			inside++;
		}
	}

	bool RWUJSemaphore::lock_upgrade(int id)
	/* should be optimized to eliminate waiting of no good enought transactions */
	/* guaranteed having lock for read */
	{
		bool res = true;
		assert(id >= 0);
		Mutex::Locker l(mutex);

		assert(inside != 0);
		if (inside - wait_upgraders > 1) {
		    //inside--;	/* not last */

		    /* there can be only one;) */
		    /* primary selection */
		    if (best_upgrader == -1 || id < best_upgrader)		/* i am better */
		    {
		        wait_upgraders++;
		        best_upgrader = id;
		        l.wait(upgrader_cond);
		        /* after signal re-aquire mutex */
		        wait_upgraders--;
		        /* secondary selection */
		        if (id == best_upgrader) {
					/* maybe a better one came meanwhile */
					//inside++;		/* is already inside */
					best_upgrader = -1;
				} else {
					inside--;
					res = false;
					/* i am worse, abort transaction */
					if (inside == 0 || wait_upgraders == 0) {
						_unlock(l);
					}
				}
			} else {
				inside--;
				res = false;
			}
		} else { /* last */
			//inside--;
			//it cannot be lowered to 0 because a writer or reader can get inside
			if (current_mode == Read)
				current_mode = Upgrade;
			if (best_upgrader == -1 || id < best_upgrader) {
				//inside++;
				best_upgrader = -1;	/* iam the best */
			} else {
				inside--;
				res = false;
			}
			if (wait_upgraders)
				/* should be signal after one-upgrader-waiting optimization */
				upgrader_cond.broadcast();
		}
		return res;
	}

	void RWUJSemaphore::unlock()
	{
		Mutex::Locker l(mutex);
		_unlock(l);
	}

	void RWUJSemaphore::_unlock(Mutex::Locker &)
	{
		inside--;
		if (current_mode == Read && inside == wait_upgraders) {
			/* last reader exits */
			if (wait_upgraders) {
				current_mode = Upgrade;
				/* should be signal after one-upgrader-waiting optimization */
				upgrader_cond.broadcast();
			} else if (wait_writers) {
				current_mode = Write;
				writer_cond.signal();
			} else
			    /* no one is waiting */
			    current_mode = None;
		} else if ( current_mode == Upgrade) {
			if (wait_upgraders) {
				/* Fix for elsewhere bug */
				/* should be signal after one-upgrader-waiting optimization */
				upgrader_cond.broadcast();
			}
			if (wait_writers) {
				current_mode = Write;
				writer_cond.signal();
			} else if (wait_readers) {
				current_mode = Read;
				/* signals all readers */
				reader_cond.broadcast();
			} else 
				current_mode = None;
		} else if ( current_mode == Write) {
			assert(inside == 0 && wait_upgraders == 0);
			if (wait_readers) {
				/* just writer access should be implemented here */
				current_mode = Read;
				/* signals all readers */
				reader_cond.broadcast();
			} else if (wait_writers) {
				current_mode = Write;
				writer_cond.signal();
			} else 
				current_mode = None;
		}
	}

}

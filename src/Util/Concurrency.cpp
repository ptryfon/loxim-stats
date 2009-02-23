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

	Mutex::Mutex()
	{
		int err;
		if ((err = pthread_mutex_init(&mutex, 0)))
			throw LoximException(err);
	}

	Mutex::~Mutex()
	{
		pthread_mutex_destroy(&mutex);
		//XXX log it somewhere if it fails
	}

	void Mutex::lock()
	{
		int error = pthread_mutex_lock(&mutex);
		if (error)
			throw LoximException(error);
	}

	void Mutex::unlock()
	{
		int error = pthread_mutex_unlock(&mutex);
		if (error)
			throw LoximException(error);
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


	Locker::Locker(Mutex &mutex) : mutex(mutex)
	{
		mutex.lock();
	}


	Locker::~Locker()
	{
		try {
			mutex.unlock();
		} catch (...) {
			assert(false);
			//do some reporting
		}
	}

	void Locker::wait(CondVar &cond)
	{	
		cond.wait(mutex);
	}
	
	bool Locker::timed_wait(CondVar &cond, struct timespec &t)
	{	
		cond.timed_wait(mutex, t);
	}
	
	Unlocker::Unlocker(Mutex &mutex) : mutex(mutex)
	{
		mutex.unlock();
	}


	Unlocker::~Unlocker()
	{
		try {
			mutex.lock();
		} catch (...) {
			assert(false);
			//do some reporting
		}
	}

}

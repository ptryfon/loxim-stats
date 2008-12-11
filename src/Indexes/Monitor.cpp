#include <Indexes/Monitor.h>

using namespace Errors;

namespace Indexes {
	
	Monitor::Monitor() {
		pthread_mutex_init(&mutex, NULL); //nie zwraca kodu bledu
		pthread_cond_init(&cond, NULL); //nie zwraca kodu bledu
	}
	
	int Monitor::lock() {
		int err;
		if ((err = pthread_mutex_lock(&mutex))) {
			return err | ErrIndexes;
		}
		return 0;
	}
	
	int Monitor::unlock() {
		int err;
		if ((err = pthread_mutex_unlock(&mutex))) {
			return err | ErrIndexes;
		}
		return 0;
	}
	
	int Monitor::wait() {
		int err;
		if ((err = pthread_cond_wait(&cond, &mutex))) {
			return err | ErrIndexes;
		}
		return 0;
	}
	
	int Monitor::signal() {
		int err;
		if ((err = pthread_cond_signal(&cond))) {
			return err | ErrIndexes;
		}
		return 0;
	}
	
	Monitor::~Monitor() {
		pthread_cond_destroy(&cond);
		pthread_mutex_destroy(&mutex);
	}
}

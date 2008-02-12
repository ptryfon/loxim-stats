#include <pthread.h>

#include "Mutex.h"
#include "../Errors/ErrorConsole.h"
#include "../Errors/Errors.h"

/*
 *	Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace SemaphoreLib {

	Mutex::Mutex()
	{
	}
	
	Mutex::~Mutex()
	{
		pthread_mutex_destroy( &mutex );
	}

	int Mutex::init(pthread_mutexattr_t* attr)
	{
		int ret = pthread_mutex_init( &mutex, attr);
		if (ret) return ErrTManager | EMutexInit;
		return ret;
	}

	int Mutex::init() {
		return init(NULL);
	}
	
	int Mutex::up()
	{
		int ret = pthread_mutex_unlock( &mutex );
		return ret;
	}

	int Mutex::down()
	{
		int ret = pthread_mutex_lock( &mutex );
		return ret;
	}
	
	
	int RecursiveMutex::init() {
		int err = 0;
		if ((err = pthread_mutexattr_init(&attr))) {
			return err | ErrTManager;
		}
		if ((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))) {
			return err | ErrTManager;
		}
		return Mutex::init(&attr);
	}
	
	RecursiveMutex::~RecursiveMutex() {
		pthread_mutexattr_destroy(&attr);
	}

} /* namespace SemaphoreLib */

#include <pthread.h>

#include "Mutex.h"
#include "../Errors/ErrorConsole.h"
#include "../Errors/Errors.h"

/*
 *	Julian Krzemiñski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace SemaphoreLib {

	Mutex::Mutex()
	{
	}
	
	Mutex::~Mutex()
	{
		pthread_mutex_destroy( &mutex );
	}

	int Mutex::init()
	{
		int ret = pthread_mutex_init( &mutex, NULL);
		if (ret) return ErrTManager | EMutexInit;
		return ret;
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

} /* namespace SemaphoreLib */

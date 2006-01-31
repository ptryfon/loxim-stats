#include <pthread.h>
#include "RWSemaphore.h"
#include "../Errors/Errors.h"

namespace SemaphoreLib {

	RWSemaphore::RWSemaphore()
	{
		current_mode	=	None;
		inside		=	0;
		wait_readers	=	0;
		wait_writers	=	0;
	}

	RWSemaphore::~RWSemaphore()
	{
		current_mode	=	None;
		inside		=	0;
	}

	int RWSemaphore::init()
	{
		int ret = 0;
		ret = pthread_mutex_init(&mutex, NULL);
		if (ret)
		{
			return ErrTManager | ESemaphoreInit;
		}
		ret = pthread_cond_init(&reader_cond, NULL);
		if (ret)
		{
			return ErrTManager | ESemaphoreInit;
		}
		ret = pthread_cond_init(&writer_cond, NULL);
		if (ret)
		{
			return ErrTManager | ESemaphoreInit;
		}
		return ret;
	}

	int RWSemaphore::destroy()
	{
		return 0;
	}

	int RWSemaphore::lock_read()
	{
		pthread_mutex_lock( &mutex );
		
		if (current_mode == None)	current_mode = Read;

		if (current_mode == Read && wait_writers == 0)
		/* reader can read - neither writer inside nor waiting */
		{
			inside++;
		}
		else 
		/* writer waiting or inside - reader must wait */
		{
			wait_readers++;
			pthread_cond_wait( &reader_cond, &mutex);
			/* after signal re-aquire mutex */
			wait_readers--;
			inside++;
		}
		pthread_mutex_unlock(&mutex);
		return 0;
	}
	
	int RWSemaphore::lock_write()
	{
		pthread_mutex_lock( &mutex );

		if (current_mode == None)	current_mode = Write;

		if (inside == 0)
		{
			inside++;
		}
		else
		{
			wait_writers++;
			pthread_cond_wait( &writer_cond, &mutex);
			/* after signal re-aquire mutex */
			wait_writers--;
			inside++;
		}
		pthread_mutex_unlock( &mutex );
		return 0;
	}

	int RWSemaphore::unlock()
	{
		pthread_mutex_lock( &mutex );
		
		inside--;
		if (current_mode == Read && inside == 0)
		/* last reader exits */
		{
			if (wait_writers)
			{
				current_mode = Write;
				pthread_cond_signal( &writer_cond );
			}
			else
			/* no one is waiting */
			current_mode = None;
		}
		else if ( current_mode == Write)
		/* quaranteed inside = 0  */
		{
			if (wait_readers)
			{			
				current_mode = Read;
				/* signals all readers */
				pthread_cond_broadcast( &reader_cond );
			}
			else if (wait_writers)
			{
				current_mode = Write;
				pthread_cond_signal( &writer_cond );
			}
			else 
				current_mode = None;
		}
		pthread_mutex_unlock( &mutex );
		return 0;
	}

	int RWSemaphore::lock_upgrade(int id)
	{
	    return ErrTManager | EUpgradeLock;
	}
} /* namespace SemaphoreLib */

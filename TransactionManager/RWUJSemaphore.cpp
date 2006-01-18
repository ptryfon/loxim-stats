#include <pthread.h>
#include "RWUJSemaphore.h"

namespace SemaphoreLib {

	RWUJSemaphore::RWUJSemaphore()
	{
		current_mode	=	None;
		inside		=	0;
		wait_readers	=	0;
		wait_writers	=	0;
		wait_updaters	=	0;
		best_upgrader	=	0;
	}

	RWUJSemaphore::~RWUJSemaphore()
	{
		current_mode	=	None;
		inside		=	0;
		best_upgrader	=	0;
	}

	int RWUJSemaphore::init()
	{
		int ret = 0;
		ret = pthread_mutex_init(&mutex, NULL);
		if (ret)
		{
			return -1;	// Error code 
		}
		ret = pthread_cond_init(&reader_cond, NULL);
		if (ret)
		{
			return -1;	// Error code
		}
		ret = pthread_cond_init(&writer_cond, NULL);
		if (ret)
		{
			return -1;	//Error code
		}
		ret = pthread_cond_init(&upgrader_cond, NULL);
		if (ret)
		{
			return -1;	//Error code
		}
		return ret;
	}

	int RWUJSemaphore::destroy()
	{
		return 0;
	}

	int RWUJSemaphore::lock_read()
	{
		pthread_mutex_lock( &mutex );
		
		if (current_mode == None)	current_mode = Read;

		if (current_mode == Read && wait_writers == 0 && wait_upgraders == 0)
		/* reader can read - neither writer inside nor waiting */
		{
			inside++;
		}
		else 
		/* writer or updater is waiting or is inside - reader must wait */
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
	
	int RWUJSemaphore::lock_write()
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

	int RWUJSemaphore::lock_upgrade(int id)
	/* should be optimized to eliminate waiting of no good enought transactions */
	{
	    int errorCode;
	    
		errorCode = 0;
	    
		pthread_mutex_lock( &mutex );

		if (current_mode == None)	current_mode = Upgrade;

		if (inside == 0)
		{
			inside++;
		}
		else
		{
			/* there can be only one;) */
			/* primary selection */
			if (best_upgrader = 0 || id < best_upgrader)		/* i am better */
			{
			    wait_upgraders++;
			    best_upgrader = id;
			    pthread_cond_wait( &upgrader_cond, &mutex);
			    /* after signal re-aquire mutex */
			    wait_upgraders--;
			    /* secondary selection */
			    if (id = best_upgrader)		/* maybe better come meanwhile */
			    {
    				inside++;
			    }
			    else errorCode = -1;	/* i am worse, abort transaction */
			}
			else errorCode = -1;	/* i am worse, abort transaction */
		}
		pthread_mutex_unlock( &mutex );
		return errorCode;
	}

	int RWUJSemaphore::unlock()
	{
		pthread_mutex_lock( &mutex );
		
		inside--;
		if (current_mode == Read && inside == 0)
		/* last reader exits */
		{
			if (wait_upgraders)
			{
				current_mode = Upgrade;
				pthread_cond_broadcast( &updater_cond );	/* should be signal after one-upgrader-waiting optimization */
			}
			else if (wait_writers)
			{
				current_mode = Write;
				pthread_cond_signal( &writer_cond );
			}
			else
			/* no one is waiting */
			current_mode = None;
		}
		else if ( current_mode == Upgrade)
		/* guaranteed inside = 0 and wait_upgraders = 0 */
		{
			if (wait_writers)
			{
				current_mode = Write;
				pthread_cond_signal( &writer_cond );
			}
			else if (wait_readers)
			{
				current_mode = Read;
				/* signals all readers */
				pthread_cond_broadcast( &reader_cond );
			}
			else 
				current_mode = None;
		}
		else if ( current_mode == Write)
		/* guaranteed inside = 0 and wait_upgraders = 0 */
		{
			if (wait_readers)		/* just writer access should be implemented here */
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
} /* namespace SemaphoreLib */

#include <stdio.h>
#include <pthread.h>
#include "RWUJSemaphore.h"
#include "../Errors/ErrorConsole.h"
#include "../Errors/Errors.h"

/*
 *	Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 */
using namespace Errors;

namespace SemaphoreLib {

	RWUJSemaphore::RWUJSemaphore()
	{
		current_mode	=	None;
		inside		=	0;
		wait_readers	=	0;
		wait_writers	=	0;
		wait_upgraders	=	0;
		best_upgrader	=	-1;
	}

	RWUJSemaphore::~RWUJSemaphore()
	{
		current_mode	=	None;
		inside		=	0;
		best_upgrader	=	-1;
	}

	int RWUJSemaphore::init()
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
		ret = pthread_cond_init(&upgrader_cond, NULL);
		if (ret)
		{
			return ErrTManager | ESemaphoreInit;	
		}
		return ret;
	}

	int RWUJSemaphore::destroy()
	{
		return 0;
	}

	int RWUJSemaphore::try_lock_write() {
		pthread_mutex_lock( &mutex );
		int errorCode;
		if (current_mode != None) {
			errorCode = ErrTManager | EBUSY;
		} else {
			errorCode = this->_lock_write();
		}
		pthread_mutex_unlock(&mutex);
		return errorCode;
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
		int errorCode = this->_lock_write();
		pthread_mutex_unlock( &mutex );
		return errorCode;
	}

	int RWUJSemaphore::_lock_write() {
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
		return 0;
	}

	int RWUJSemaphore::lock_upgrade(int id)
	/* should be optimized to eliminate waiting of no good enought transactions */
	/* guaranteed having lock for read */
	{
	    int errorCode;
		errorCode = 0;
	    
		pthread_mutex_lock( &mutex );

		if (inside == 0)
		    return ErrTManager | EUpgradeLock;		/* error: lock_upgrade without having a lock */
		else if (inside - wait_upgraders > 1)
		{
		    //inside--;	/* not last */

		    /* there can be only one;) */
		    /* primary selection */
		    if (best_upgrader == -1 || id < best_upgrader)		/* i am better */
		    {
		        wait_upgraders++;
		        best_upgrader = id;
		        pthread_cond_wait( &upgrader_cond, &mutex);
		        /* after signal re-aquire mutex */
		        wait_upgraders--;
		        /* secondary selection */
		        if (id == best_upgrader)		/* maybe better come meanwhile */
		        {
    			    //inside++;		/* is already inside */
			    best_upgrader = -1;
			}
			else 
			{
			    inside--;
			    errorCode = ErrTManager | EUpgradeLock;	/* i am worse, abort transaction */
			    if (inside == 0 || wait_upgraders == 0)
			    {
				this->_unlock();
			    }
			}
		    }
		    else
		    {
			inside--;
			errorCode = ErrTManager | EUpgradeLock;	/* i am worse, abort transaction */
		    }
		}
		else		/* last */
		{
		    //inside--;		//its cannot be lower to 0 because then writer or reader can get inside
		    if (current_mode == Read)	current_mode = Upgrade;
		    if (best_upgrader == -1 || id < best_upgrader)
		    {
			//inside++;
			best_upgrader = -1;	/* iam the best */
		    }
		    else
		    {
			inside--;
			errorCode = ErrTManager | EUpgradeLock;
		    }
		    if (wait_upgraders)
    			pthread_cond_broadcast( &upgrader_cond );	/* should be signal after one-upgrader-waiting optimization */
		}
		pthread_mutex_unlock( &mutex );
		return errorCode;
	}

	int RWUJSemaphore::status()	//Test routine
	{
	    ErrorConsole ec("TransactionManager");

	    printf("Inside %d\n", inside);
	    printf("Readers waiting %d\n", wait_readers);
	    printf("Upgraders waiting %d\n", wait_upgraders);
	    printf("Writers waintin %d\n", wait_writers);
	    printf("Best upgrader %d\n", best_upgrader);
	    printf("Current mode %d\n", current_mode);
	    return 0;
	}

	int RWUJSemaphore::unlock()
	{
		pthread_mutex_lock( &mutex );
		int errorCode = this->_unlock();
		pthread_mutex_unlock( &mutex );
		return errorCode;
	}

	int RWUJSemaphore::_unlock()
	/* guaranteed having mutex */
	{
//		pthread_mutex_lock( &mutex );
		
		inside--;
		if (current_mode == Read && inside == wait_upgraders)
		//if (current_mode == Read && inside == 0)
		/* last reader exits */
		{
			if (wait_upgraders)
			{
				current_mode = Upgrade;
				pthread_cond_broadcast( &upgrader_cond );	/* should be signal after one-upgrader-waiting optimization */
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
		/* guaranteed inside = 0 */
		{
			if (wait_upgraders)
			{
				/* Fix for elsewhere bug */
				pthread_cond_broadcast( &upgrader_cond );	/* should be signal after one-upgrader-waiting optimization */
			}
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
//		pthread_mutex_unlock( &mutex );
		return 0;
	}
} /* namespace SemaphoreLib */

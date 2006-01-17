#ifndef _RWSEMAPHORE_
#define _RWSEMAPHORE_

#include <pthread.h>
#include <stdlib.h>
#include "Semaphore.h"

namespace SemaphoreLib {

class RWUJSemaphore: public Semaphore
{
	private:
		pthread_mutex_t	mutex;
		pthread_cond_t  reader_cond;
		pthread_cond_t	writer_cond;
		pthread_cond_t	changer_cond;

		enum LockMode {
			Read	=	0x01,
			Write	=	0x02,
			Update	=	0x03,
			None	=	0x09
		};

		LockMode current_mode;
		int wait_writers;
		int wait_readers;
		int wait_updaters;
	
		int inside;		
	public:
		RWUJSemaphore();
		virtual ~RWUJSemaphore();

		virtual int init();
		virtual int destroy();
		virtual int lock_read();
		virtual int lock_write();
		virtual int unlock();
		virtual int lock_update();
};


} /* namespace SemaphoreLib*/

#endif

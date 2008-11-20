#ifndef _RWSEMAPHORE_
#define _RWSEMAPHORE_

#include <pthread.h>
#include <stdlib.h>
#include "Semaphore.h"

/*
 *	Julian Krzemiñski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace SemaphoreLib {

class RWSemaphore: public Semaphore
{
	private:
		pthread_mutex_t	mutex;
		pthread_cond_t  reader_cond;
		pthread_cond_t	writer_cond;

		enum LockMode {
			Read	=	0x01,
			Write	=	0x02,
			None	=	0x09
		};

		LockMode current_mode;
		int wait_writers;
		int wait_readers;
	
		int inside;		
	public:
		RWSemaphore();
		virtual ~RWSemaphore();

		virtual int init();
		virtual int destroy();
		virtual int lock_read();
		virtual int lock_write();
		virtual int unlock();
		virtual int lock_upgrade(int id);
};


} /* namespace SemaphoreLib*/

#endif

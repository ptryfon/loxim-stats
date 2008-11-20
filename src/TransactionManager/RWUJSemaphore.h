/*
  Readers-Writers-Upgraders semaphores with Just access for writers
*/

#ifndef _RWUJSEMAPHORE_
#define _RWUJSEMAPHORE_

#include <pthread.h>
#include <stdlib.h>
#include "Semaphore.h"

/*
 *	Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace SemaphoreLib {

class RWUJSemaphore: public Semaphore
{
	private:
		pthread_mutex_t	mutex;
		pthread_cond_t  reader_cond;
		pthread_cond_t	writer_cond;
		pthread_cond_t	upgrader_cond;

		enum LockMode {
			Read	=	0x01,
			Write	=	0x02,
			Upgrade	=	0x03,
			None	=	0x09
		};

		LockMode current_mode;
		int wait_writers;
		int wait_readers;
		int wait_upgraders;
		int best_upgrader;	/* abstract id of provided by objects using semaphores
					   here: Transactions provide TransactionID */
	
		int inside;
		int _unlock();
		int _lock_write();
	public:
		RWUJSemaphore();
		virtual ~RWUJSemaphore();

		virtual int init();
		virtual int destroy();
		virtual int lock_read();
		virtual int lock_write();
		virtual int try_lock_write();
		virtual int unlock();
		virtual int lock_upgrade(int id);
		int status();
};


} /* namespace SemaphoreLib*/

#endif

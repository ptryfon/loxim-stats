
#include "SingleLock.h"

namespace LockMgr
{

/*_____SingleLock___________________________________________________*/    

    SingleLock::SingleLock(TransactionID* tid, AccessMode mode, Semaphore *_sem, int _id)
    {
	inside 	= 1;	
	id 	= _id;
	sem 	= _sem;	

	this->current_mode = mode;
	
	if (mode == Read ) sem->lock_read();
	if (mode == Write) sem->lock_write();

	/* set of transactions that locked an object */
	current = new TransactionIdSet;
	current->insert(*tid);

	mutex = new Mutex();
	mutex->init();
    }

    SingleLock::~SingleLock()
    {
	delete current;
    }

    int SingleLock::wait_for_lock(TransactionID * tid, AccessMode mode)
    {
	if (mode == Read) 
		sem->lock_read();
	else 
		sem->lock_write();

	mutex->down();
		
		current->insert(*tid);
		inside++;

	mutex->up();
	
   	return 0;
    }

    int SingleLock::unlock(TransactionID* tid)
    {
	int delete_lock = 0;  /* if true - object SingleLock will be destroyed afterwards */
	
	mutex->down();

		inside--;
		current->erase(*tid);
		
	mutex->up();

	sem->unlock();

	return delete_lock;
    }
    
    int SingleLock::getId() const
    {
    	return id;
    }
    
}  /* namespace LockMgr */


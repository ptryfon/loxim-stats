
#include "SingleLock.h"

namespace LockMgr
{

/*_____SingleLock___________________________________________________*/    

    SingleLock::SingleLock(TransactionID* _tid, AccessMode _mode, Semaphore *_sem, int _id)
    {
	inside 	= 1;	
	id 	= _id;
	sem 	= _sem;		
	tid	= _tid;
	waiting = 0;
	
	if ( _mode == Read ) sem->lock_read();
	if ( _mode == Write) sem->lock_write();

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

    int SingleLock::wait_for_lock(TransactionID *_tid, AccessMode _mode)
    {
	/* prevention against deadlocks: strategy wait - die */
	mutex->down();		
		if ( tid->getId() < _tid->getId() ) 
		/* younger dies ( younger = higher id ) */
		{
			mutex->up();
			return -1; // error abort (rollback) transction
		}
		
	int isCurrent = current->find(*_tid) != current->end();
	waiting++;
	
	mutex->up();
	
	if (_mode == Read && !isCurrent) 
	{		
		sem->lock_read();
	}
	else if (_mode == Write && isCurrent)
	{
		sem->lock_upgrade(_tid->getId());
	}
	else if (_mode == Write && !isCurrent)
	{
		sem->lock_write();
	}

	mutex->down();
	
		waiting--;	
		current->insert(*_tid);
		tid = _tid;
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
		
		if (inside == 0 && waiting == 0)	
		    delete_lock = 1;
		
	mutex->up();

	sem->unlock();

	return delete_lock;
    }
    
    int SingleLock::getId() const
    {
    	return id;
    }
    
}  /* namespace LockMgr */


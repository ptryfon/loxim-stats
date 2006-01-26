
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
	currentRead = new TransactionIdSet;
	currentWrite = new TransactionIdSet;
	if (_mode == Read)
	    currentRead->insert(*tid);
	else
	    currentWrite->insert(*tid);

	mutex = new Mutex();
	mutex->init();
    }

    SingleLock::~SingleLock()
    {
	delete currentRead;
	delete currentWrite;
    }

    int SingleLock::wait_for_lock(TransactionID *_tid, AccessMode _mode)
    {
	int errorCode;
	errorCode = 0;
	/* prevention against deadlocks: strategy wait - die */
	mutex->down();
		if ( tid->getId() < _tid->getId() )
		/* younger dies ( younger = higher id ) */
		{
			mutex->up();
			return -1; // error abort (rollback) transction
		}

	int isCurrentRead = currentRead->find(*_tid) != currentRead->end();
	int isCurrentWrite = currentWrite->find(*_tid) != currentWrite->end();
	waiting++;

	mutex->up();

	if (_mode == Read && !isCurrentRead)
	{		
		errorCode = sem->lock_read();
	}
	else if (_mode == Write && isCurrentRead && !isCurrentWrite)
	{
		errorCode = sem->lock_upgrade(_tid->getId());
	}
	else if (_mode == Write && !isCurrentWrite)
	{
		errorCode = sem->lock_write();
	}

	if (errorCode == 0)
	{
	    mutex->down();
	
		waiting--;
		if (_mode == Read)
		    currentRead->insert(*_tid);
		else
		    currentWrite->insert(*_tid);
		tid = _tid;
		inside++;

	    mutex->up();
	}
   	return errorCode;
    }

    int SingleLock::unlock(TransactionID* tid)
    {
	int delete_lock = 0;  /* if true - object SingleLock will be destroyed afterwards */
	
	mutex->down();

		inside--;
		if (currentRead->find(*tid) != currentRead->end())
		    currentRead->erase(*tid);
		if (currentWrite->find(*tid) != currentWrite->end())
		    currentWrite->erase(*tid);
		
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


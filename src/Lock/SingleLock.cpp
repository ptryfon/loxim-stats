
#include <Lock/SingleLock.h>

using namespace Util;

/**
 *	@author Julian Krzemiñski (julian.krzeminski@students.mimuw.edu.pl)
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 */
namespace LockMgr
{

/*_____SingleLock___________________________________________________*/    

    SingleLock::SingleLock(TransactionID* _tid, AccessMode _mode, RWUJSemaphore *_sem, int _id) : 
	    err(ErrorConsole::get_instance(EC_LOCK_MANAGER))
    {
	inside 	= 1;	
	id 	= _id;
	sem 	= _sem;		
	tid	= _tid;
	waiting = 0;

	if ( _mode == Read ) sem->lock_read();
	if ( _mode == Write) sem->lock_write();

	/* set of transactions that locked an object */
	currentRead  = new TransactionIdSet;
	currentWrite = new TransactionIdSet;
	
	if (_mode == Read)
	    currentRead->insert(tid);
	else
	    currentWrite->insert(tid);

	mutex = new Mutex();
	
    }

    SingleLock::~SingleLock()
    {
	debug_printf(err, "SingleLock destructor\n");
	delete mutex;
	delete sem;
	
	debug_printf(err, "SingleLock destructor - free structures memory \n");
	/* quaranteed to be empty */
	delete currentRead;
	delete currentWrite;
    }

    int SingleLock::preventDeadlock(TransactionID* _tid)
    {
//oldversion	if ( tid->getId() < _tid->getId() )
	if ( tid->getPriority() < _tid->getPriority() )
	/* younger dies ( younger = higher id ) */
	{
	    /* deadlock exception - rollback transaction */
	    debug_printf(err, "Deadlock exception\n");
	    return ErrTManager | EDeadlock; 
	}
	else return 0;
    }

    /**
     * basic lock operation - determines whether transaction already has a lock,
     * in what mode and on those basis the exact semaphore operation is choosen
     * deadlock prevetion is also here
     */
    int SingleLock::wait_for_lock(TransactionID *_tid, AccessMode _mode)
    {
	int errorCode;
	errorCode = 0;
	/* prevention against deadlocks: strategy wait - die */
	int isCurrentRead;
	int isCurrentWrite;
	{
		Mutex::Locker l(*mutex);

		isCurrentRead  = currentRead->find(_tid)  != currentRead->end();
		isCurrentWrite = currentWrite->find(_tid) != currentWrite->end();
		waiting++;
	}
	/* here we lost control on _mode, should be fixed later!!! */

	debug_printf(err, "Wait for lock, tid = %d, mode = %d\n", _tid->getId(), _mode);
	if (_mode == Read && !isCurrentRead && !isCurrentWrite)
	{
		/* many readers -> OK, reader and writer -> prevent deadlock */
		if (currentWrite->empty())
		    sem->lock_read();
		else {
		    if ((errorCode = preventDeadlock(_tid)) == 0)
			sem->lock_read();
		}
	}
	else if (_mode == Read && isCurrentRead)	/* reader inside */
	{
		waiting--;
		return 0;
	}
	else if (_mode == Read && isCurrentWrite)	/* this transaction holds object in exclusive access mode */
	{
		waiting--;
		return 0;
	}
	else if (_mode == Write && !isCurrentRead && !isCurrentWrite)
	{
		if ((errorCode = preventDeadlock(_tid)) == 0)
		    sem->lock_write();
	}
	else if (_mode == Write && isCurrentWrite)	/* this transaction holds object in exclusive access mode */
	{
		waiting--;
		return 0;
	}
	else if (_mode == Write && isCurrentRead)	/* upgrade lock from read to write */
	{
		debug_printf(err, "Standard upgrade");
		if ((errorCode = preventDeadlock(_tid)) == 0)
		    errorCode = sem->lock_upgrade(_tid->getPriority());
//oldversion		    errorCode = sem->lock_upgrade(_tid->getId());
	}
	
	debug_printf(err, "Locking object, tid = %d\n", _tid->getId());
	if (errorCode == 0)
	{
	    Mutex::Locker l(*mutex);
	
		waiting--;
		if (_mode == Read)
		{
		    debug_printf(err, "New Reader, tid = %d\n", _tid->getId());
		    currentRead->insert(_tid);
		}
		else
		{
		    debug_printf(err, "New Writer, tid = %d\n", _tid->getId());
		    currentWrite->insert(_tid);
		}
		/* for readers we keep the oldest transaction id */
//oldversion		if (_tid->getId() < tid->getId()) tid = _tid;
		if (_tid->getPriority() < tid->getPriority()) tid = _tid;
		inside++;
	}
   	return errorCode;
    }

    int SingleLock::unlock(TransactionID* tid)
    {
	int delete_lock = 0;  /* if true - object SingleLock will be destroyed afterwards */
	
	{
		Mutex::Locker l(*mutex);

		inside--;
		if (currentRead->find(tid) != currentRead->end())
		    currentRead->erase(tid);
		if (currentWrite->find(tid) != currentWrite->end())
		    currentWrite->erase(tid);
		
		if (inside == 0 && waiting == 0)
		    delete_lock = 1;
	}

	sem->unlock();

	return delete_lock;
    }
    
    int SingleLock::getId() const
    {
    	return id;
    }
    
    void SingleLock::setPHID(DBPhysicalID* dbid)
    {
	this->phid = dbid;
    }
    DBPhysicalID* SingleLock::getPHID() const
    {
	return phid;
    }
}  /* namespace LockMgr */


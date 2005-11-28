#include <stdio.h>
#include "Lock.h"

namespace LockMgr{


/*_____LockManager___________________________________________________*/
    LockManager* LockManager::lockMgr = new LockManager();

    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {        
	return 0;
    }
    int LockManager::unlockAll(TransactionID* transaction_id)
    {
	set<DBPhysicalID*>* locks = (*transaction_locks)[transaction_id];
	for (set<DBPhysicalID*>::iterator iter = locks->begin();
	     iter != locks->end(); iter++)
	{
	    unlock(*iter);
	}
	return 0;
    }
    int LockManager::unlock(DBPhysicalID* pid)
    {
	return 0;
    }
    LockManager* LockManager::getHandle() { return lockMgr; }
    
    LockManager::LockManager() 
    {
	transaction_locks = new map<TransactionID* , set<DBPhysicalID*>*>;
	map_of_locks      = new map<DBPhysicalID*, SingleLock*>;
    }


/*_____SingleLock___________________________________________________*/    
    SingleLock::SingleLock(int current, AccessMode mode)
    {
	if ( mode == Read)
	{
	    count_wait_writers = 0;
	    count_wait_readers = 1;
	}
	else
	{
	    count_wait_readers = 0;
	    count_wait_writers = 1;
	}
	this->current_mode = mode;
	wait_readers = create_sem(SEMKEY2);
	wait_writers = create_sem(SEMKEY3);
    }
    int SingleLock::is_reader_waiting()
    {
	return count_wait_readers > 0;
    }
    int SingleLock::is_writer_waiting()
    {
	return count_wait_writers > 0;
    }
    void SingleLock::setCurrent(TransactionID* current, AccessMode mode)
    {
	this->current_mode = mode;
    }
    SingleLock::~SingleLock()
    {
	release_sem(SEMKEY2);
	release_sem(SEMKEY2);
    }
}

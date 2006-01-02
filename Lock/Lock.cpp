
#include "Lock.h"

namespace LockMgr
{

/*_____LockManager___________________________________________________*/


    /* the only instance of class */
    LockManager* LockManager::lockMgr = new LockManager();

    LockManager* LockManager::getHandle() { return lockMgr; }

    LockManager::LockManager() 
    {
		err = ErrorConsole();
		transaction_locks = new TransactionIdMap;
		map_of_locks      = new DBPhysicalIdMap; 
		single_lock_id	  = 0;
		mutex = new Mutex();
		mutex->init();
    }

    LockManager::~LockManager()
    {
		delete mutex;
		for (TransactionIdMap::iterator iter = transaction_locks->begin();
	    	iter != transaction_locks->end(); iter++ )
	
	    	delete (iter->second);	
    }
        
    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {  
		int errorNumber = 0;
		mutex->down();
	    	DBPhysicalID* phid = (DBPhysicalID*) lid->getPhysicalID();
	    	DBPhysicalIdMap::iterator pos = map_of_locks->find(*phid);
	    
	    	if (pos == map_of_locks->end())
	    	{
			printf("TID new lock: %d\n", tid->getId() );fflush(stdout);
			/* creating new single lock */
			SingleLock* lock = new SingleLock(tid, mode, new RWSemaphore(), single_lock_id);

			single_lock_id++;
			(*map_of_locks)[*phid] = lock;	
			(*transaction_locks)[*tid] = new SingleLockSet;
			((*transaction_locks)[*tid])->insert(*lock);
			mutex->up();	
	    	}
	    	else
	    	{
			printf("TID wait lock: %d\n", tid->getId() );fflush(stdout);			
			/* modifying existing single lock for this PhysicalID */
			mutex->up();
			SingleLock* lock = pos->second;
			errorNumber = lock->wait_for_lock(tid, mode);
		}
		    
		return errorNumber;
    }

    int LockManager::unlockAll(TransactionID* transaction_id)
    {
		mutex->down();
		TransactionIdMap::iterator pos = transaction_locks->find(*transaction_id);
		if (pos != transaction_locks->end())
		{
		    SingleLockSet* locks = pos->second;
		    for (SingleLockSet::iterator iter = locks->begin();
		        iter != locks->end(); iter++)
		    {
				int delete_lock = unlock((*iter), transaction_id);
				if (delete_lock)
					delete &(*iter);
		    }
		}	
	
		mutex->up();
		return 0;
    }
    
    int LockManager::unlock(SingleLock lock, TransactionID* tid)
    {
		return lock.unlock(tid);
    }
}

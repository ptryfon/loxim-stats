
#include "Lock.h"

/**
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 *	@author Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace LockMgr
{


/*_____LockManager___________________________________________________*/


    /* the only instance of class */
    LockManager* LockManager::lockMgr = NULL;

    LockManager* LockManager::getHandle() { return lockMgr; }
    
    void LockManager::init() {
    	lockMgr = new LockManager();;
    }
    
    LockManager::LockManager() 
    {
		err = ErrorConsole("LockManager");
		transaction_locks = new TransactionIdMap;
		map_of_locks      = new DBPhysicalIdMap; 
		single_lock_id	  = 0;
		mutex = new Mutex();
		mutex->init();
    }

    LockManager::~LockManager()
    {
		err.printf("LockManager - destructor\n");
		delete mutex;
		
		/* 
		 * TransactionManager removes all locks that is why structures:
		 * transaction_locks, map_of_locks should be empty, 
		 * hovever, just in case, we erase (lost) elements from these structures
		 */
		for (TransactionIdMap::iterator iter = transaction_locks->begin();
	    	iter != transaction_locks->end(); iter++ )
	
	    	delete (iter->second);	
		
		delete transaction_locks;
		delete map_of_locks;
    }
        
    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {
		int errorCode = 0;
      
		mutex->down();
		
		errorCode = lock_primitive(lid, tid, mode);
		
		mutex->up();
		
		return errorCode;
    }

    int LockManager::lock_primitive(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {  
		int errorNumber = 0;

	    	DBPhysicalID* phid = lid->getPhysicalID();
	    	DBPhysicalIdMap::iterator pos = map_of_locks->find(phid);
	    
		SingleLock* lock = NULL;
		
	    	if (pos == map_of_locks->end())
	    	{
			err.printf("New lock, tid = %d\n", tid->getId() );
			/* creating new single lock */
			RWUJSemaphore* rwsem = new RWUJSemaphore();
			rwsem->init();
			lock = new SingleLock(tid, mode, rwsem, single_lock_id);
			lock->setPHID(phid);
			
			single_lock_id++;
			(*map_of_locks)[phid] = lock;	
	    	}
	    	else
	    	{
			err.printf("Wait for lock, tid = %d\n", tid->getId() );
			/* modifying existing single lock for this PhysicalID */
			mutex->up();
			lock = pos->second;
			errorNumber = lock->wait_for_lock(tid, mode);
			
			if (errorNumber) return errorNumber;
			
			mutex->down();
		}
		err.printf("Lock received\n");
		if ((*transaction_locks)[tid] == 0 )
		    (*transaction_locks)[tid] = new SingleLockSet;
			    
		((*transaction_locks)[tid])->insert(lock);
			
		
		return errorNumber;
    }

    int LockManager::lockAll(set<LogicalID*>* lock_set, TransactionID* tid, AccessMode mode)
    {
		int errorCode = 0;

		err.printf("LockAll, tid = %d\n", tid->getId());
		mutex->down();
		
		set<LogicalID*>::iterator pos = lock_set->begin();
		while(!errorCode && pos != lock_set->end())
		{
		    errorCode = lock_primitive(*pos, tid, mode);
		}

		mutex->up();
		return errorCode;
    }


    int LockManager::unlockAll(TransactionID* transaction_id)
    {
		err.printf("UnlockAll, tid = %d\n", transaction_id->getId());
		mutex->down();
		TransactionIdMap::iterator pos = transaction_locks->find(transaction_id);
		if (pos != transaction_locks->end())
		{
		    SingleLockSet* locks = pos->second;
		    for (SingleLockSet::iterator iter = locks->begin();
		        iter != locks->end(); iter++)
		    {
				err.printf("Unlock single, tid = %d\n", transaction_id->getId());
				int delete_lock = unlock((*iter), transaction_id);
				
				if (delete_lock) 
				{
				    DBPhysicalID* phid = (*iter)->getPHID();
				    map_of_locks->erase( phid );
				    
				    delete (*iter);				    
				}
		    }
		    transaction_locks->erase(transaction_id);		    
		    delete locks;
		}	
	
		mutex->up();
		return 0;
    }

    int LockManager::modifyLockLID(DBPhysicalID* oldPHID, LogicalID* newLID, TransactionID* tid)
    {
	int errorNumber = 0;

	err.printf("modifyLockLID, tid = %d\n", tid->getId());

	mutex->down();
	    DBPhysicalID* newPHID = newLID->getPhysicalID();
	    DBPhysicalIdMap::iterator pos = map_of_locks->find(oldPHID);
	    if (pos != map_of_locks->end())
	    {
		    SingleLock* lock = pos->second;
		    map_of_locks->erase(oldPHID);
		    lock->setPHID(newPHID);
		    (*map_of_locks)[newPHID] = lock;	
	    }
	    else errorNumber = -1;
	
	mutex->up();

	return errorNumber;
    }
    
    int LockManager::unlock(SingleLock* lock, TransactionID* tid)
    {
		return lock->unlock(tid);
    }
}


#include "Lock.h"

namespace LockMgr
{

    /* returns true if db1 > db2  */
   bool DBPhysicalIDCmp::operator() (const DBPhysicalID& db1, const DBPhysicalID& db2) const 
   {
		unsigned short f1, f2, p1, p2, o1, o2;
		f1 = db1.getFile();
		f2 = db2.getFile();
	    
		if (f1 != f2) return f1<f2;
		    
		p1 = db1.getPage();
		p2 = db2.getPage();
		    
		if (p1 != p2) return p1<p2;
	    
		o1 = db1.getOffset();
		o2 = db2.getOffset();
	    
		return o1<o2;	
   }

    /* returns true if tid1 > tid2 */
   bool TransactionIDCmp::operator() (const TransactionID& tid1, const TransactionID& tid2) const 
   {
		return ( tid1.getId() > tid2.getId());
   }
   
   /* returns true if sl_1 > sl_2 */
   bool SingleLockCmp::operator() (const SingleLock& sl_1, const SingleLock& sl_2) const 
   {
		return ( sl_1.getId() > sl_2.getId() );
   }


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
}

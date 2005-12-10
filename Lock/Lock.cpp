#include <stdio.h>
#include "Lock.h"
#include <iomanip>
#include <algorithm>

using namespace Errors;
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
		current_sem_key	  = 2064;
		SEMKEY = 2002;
		if ((mutex = create_sem(SEMKEY)) < 0)
		    err << "LockManager: Cannot create sem\n" ;
		V(mutex);
    }

    LockManager::~LockManager()
    {
		for (TransactionIdMap::iterator iter = transaction_locks->begin();
	    	iter != transaction_locks->end(); iter++ )
	
	    	delete (iter->second);	
    }
        
    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {  
		int errorNumber = 0;
		P(mutex);
	    	DBPhysicalID* phid = (DBPhysicalID*) lid->getPhysicalID();
	    	DBPhysicalIdMap::iterator pos = map_of_locks->find(*phid);
	    
	    	if (pos == map_of_locks->end())
	    	{
				/* creating new single lock */
				SingleLock* lock = new SingleLock(tid, mode, current_sem_key);
				/* values: 
				 *  current_sem	   : used for mutual exclusion    
				 * 	current_sem+1  : used for semaphore for reading
		 		 * 	current_sem+2  : used for semaphore for writing
		 	 	 */
				current_sem_key += 8;
				(*map_of_locks)[*phid] = lock;	
				(*transaction_locks)[*tid] = new SingleLockSet;
				((*transaction_locks)[*tid])->insert(*lock);
				V(mutex);	
	    	}
	    	else
	    	{
				/* modifying existing single lock for this PhysicalID */
				V(mutex);
				SingleLock* lock = pos->second;
				errorNumber = lock->wait_for_lock(tid, mode);
		    }
		    
		return errorNumber;
    }
    int LockManager::unlockAll(TransactionID* transaction_id)
    {
		P(mutex);
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
	
		V(mutex);
		return 0;
    }
    
    int LockManager::unlock(SingleLock lock, TransactionID* tid)
    {
		return lock.unlock(tid);
    }


/*_____SingleLock___________________________________________________*/    
    SingleLock::SingleLock(TransactionID* tid, AccessMode mode, int current_sem)
    {
  		count_wait_writers = 0;
    	count_wait_readers = 0;
		inside = 1;
		
		this->current_mode = mode;
		mutex        = create_sem(current_sem);
		wait_readers = create_sem(current_sem+1);
		wait_writers = create_sem(current_sem+2);
		str_mutex    = create_sem(current_sem+3); /* structure current mutex - reguire seperate sem */
		V(mutex);
		V(str_mutex);
		
		/* set of transactions that locked an object */
		current = new TransactionIdSet;
		current->insert(*tid);
    }
	SingleLock::~SingleLock()
    {
		release_sem(mutex);
		release_sem(wait_readers);
		release_sem(wait_writers);		
    }
    int SingleLock::wait_for_lock(TransactionID * tid, AccessMode mode)
    {
    	P(mutex);
    		/*
    		 * 	Algorithm : Readers and Writers
    		 *  wpuszamy czytelnikow dopoki nie czeka pisarz, przychodzi pisarz - czeka az wyjda
    		 *  czytelnicy, wychodzi pisarz - wpuszczamy wszystkich czytelnikow
    		 */ 
    		if (this->current_mode == mode && mode == Read && !is_writer_waiting())
    		{
    			current->insert(*tid);   
    			inside++; 			
    			V(mutex);
    		}
    		else
    		{
    			if (mode == Read)  
    			{ 
    				count_wait_readers++; 
    				V(mutex);
    				P(wait_readers); 
    			} 
    			else /* mode == Write */
    			{
    				count_wait_writers++; 
    				V(mutex);
    				P(wait_writers); 
    			}
    			P(str_mutex);
    				current->insert(*tid);  
    				inside++;  				
    			V(str_mutex);
    		}    	
		return 0;
    }
	int SingleLock::unlock(TransactionID* tid)
	{
		int delete_lock = 0;  /* if true - object SingleLock will be destroyed afterwards */
		P(mutex);
			/*
			 *	algorithm: Readers and Writers 
			 */
			 inside--;
			 current->erase(*tid);
			 if (this->current_mode == Write) /* wychodzi pisarz */
			 {
			 	if (is_reader_waiting())	  /* wpuszczam wszystkich czytelnikow */
			 	{			 		
			 		for(int i=0; i<count_wait_readers; i++)
			 			V(wait_readers);	
			 		count_wait_readers = 0;
			 		current_mode = Read;
			 	}
			 	else if (is_writer_waiting())
			 	{
			 		V(wait_writers);
			 		count_wait_writers--;
			 	}
			 	else
			 		delete_lock = 1;
			 }
			 else	/* wychodzi czytelnik */
			 {
			 	if (!inside) /* no other reader locks an object */
			 	{
			 		if (is_writer_waiting())
			 		{
			 			V(wait_writers);
				 		count_wait_writers--;
			 		}
			 		else
			 			delete_lock = 1;			 		
			 	}
			 }
		V(mutex);
		return delete_lock;
	}
	
    int SingleLock::is_reader_waiting()
    {
		return count_wait_readers > 0;
    }
    int SingleLock::is_writer_waiting()
    {
		return count_wait_writers > 0;
    }    
    int SingleLock::getId() const
    {
    	return mutex;
    }
}

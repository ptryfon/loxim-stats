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


/*_____LockManager___________________________________________________*/

    /* the only instance of class */
    LockManager* LockManager::lockMgr = new LockManager();

    LockManager* LockManager::getHandle() { return lockMgr; }

    LockManager::LockManager() 
    {
	err = ErrorConsole();
	sem_created = new set<int>;
	transaction_locks = new TransactionIdMap;
	map_of_locks      = new PhysicalIdMap; 
	current_sem_key	  = 2000;
	sem_created->insert(current_sem_key);
	if ((mutex = create_sem(SEMKEY2)) < 0)
	    err << "LockManager: Cannot create sem\n" ;
	V(mutex);
    }

    LockManager::~LockManager()
    {
	for (set<int>::iterator iter = sem_created->begin();
	     iter != sem_created->end(); iter++ )
	
	    release_sem(*iter);
	
    }    
    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {  
	int erno = 0;
	P(mutex);
	    DBPhysicalID* phid = (DBPhysicalID*) lid->getPhysicalID();
	    PhysicalIdMap::iterator pos = map_of_locks->find(*phid);
	    
	    if (pos == map_of_locks->end())
	    {
		/* creating new single lock */
		SingleLock* lock = new SingleLock(tid, mode, current_sem_key);
		/* values: 
		 * 	current_sem    : used for semaphore for reading
		 * 	current_sem+1  : used for semaphore for writing
		 */
		sem_created->insert(current_sem_key);
		sem_created->insert(current_sem_key+1);
		current_sem_key += 2;
		(*map_of_locks)[*phid] = lock;	
		
		V(mutex);	
	    }
	    else
	    {
		/* modifying existing single lock for this PhysicalID */
		V(mutex);
		SingleLock* lock = pos->second;
		//errno = lock->wait_for_lock(tid, mode);
	    }
	return erno;
    }
    int LockManager::unlockAll(TransactionID* transaction_id)
    {
	P(mutex);
	TransactionIdMap::iterator pos = transaction_locks->find(*transaction_id);
	if (pos != transaction_locks->end())
	{
	    set<DBPhysicalID>* locks = pos->second;
	    for (set<DBPhysicalID>::iterator iter = locks->begin();
	        iter != locks->end(); iter++)
	    {
		unlock((*iter), transaction_id);
	    }
	}
	
	V(mutex);
	return 0;
    }
    int LockManager::unlock(DBPhysicalID phid, TransactionID* tid)
    {
	/* dziedziczenie sekcji krytycznej */
	return 0;
    }


/*_____SingleLock___________________________________________________*/    
    SingleLock::SingleLock(TransactionID* tid, AccessMode mode, int current_sem)
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
	wait_readers = create_sem(current_sem);
	wait_writers = create_sem(current_sem+1);
    }
    int wait_for_lock(TransactionID * tid, AccessMode mode)
    {
	return 0;
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

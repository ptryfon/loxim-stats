#ifndef _LOCK_
#define _LOCK_

namespace LockMgr { class LockManager; }

#include "../Store/Store.h"
#include "../Store/DBPhysicalID.h"
#include "../TransactionManager/Transaction.h"
#include "../TransactionManager/Semlib.h"

#include <deque>
#include <map>
#include <set>
#include <iterator>

using namespace Store;
using namespace TManager;

namespace LockMgr {

    /* class declaration */
    class SingleLock;
    
    /*
     *	Singleton Desing Pattern
     */
    class LockManager
    {
	private:	    
	    /*
	     * The only instance of class
	     */
	    static LockManager* lockMgr;
	    
    	    LockManager();
	    
	    /* 
	     * map from PhysicalId to SingleLock 
	     */
	    map<Store::DBPhysicalID* , SingleLock* >* map_of_locks;
	    
	    /* 
	     * map from transactionId to list of PhysicalIds locked 
	     */
	    map<TransactionID* , set<DBPhysicalID*>* >* transaction_locks;
	    
	    /*
	     * transaction release object 
	     * (hovewer it can be locked by other tranasactions if share_lock was used )
	     */
	    int unlock(DBPhysicalID*, TransactionID*);
	    
	    /* For every DBPhysicalID we need two semaphores: 
	     * 	a. for readers
	     * 	b. for writers
	     * For every semaphore we need a unique number. This number 
	     * is current_sem_key - incremented with every new semaphore
	     */
	    int current_sem_key;
	    
	public: 
	    /*
	     * Returns the only instance of class
	     */
	    static LockManager* getHandle();
	    
	    /*
	     * if object is already locked Transaction will wait on a semaphore
	     */
	    int lock(LogicalID* lid, TransactionID* tid, AccessMode mode);
	    
	    /*
	     * Transaction calls commit or rollback on TransactionManager, who calls LockManager
	     * to unlock all objects related with this transaction
	     */
	    int unlockAll(TransactionID* );
    };

    /*
     *	Class representing Lock for single Object
     */
    class SingleLock
    {
	private:
	    
	    /* 
	     * current transaction ids that locked an object 
	     * (if the mode is for reading there may be many transactions that hold on object )
	     */
	    set<TransactionID*> current; 
	    
	    /* 
	     * current lock mode for object: Read | Write 
	     */
	    AccessMode current_mode;
	    
	    /* 
	     * transaction ids that wait for reading an object (wait on a semaphore with unique id = wait_readers)
	     */
	    int wait_readers;
	    
	    /* 
	     * transaction ids that wait for writing an object (wait on a semaphore with unique id = wait_writers)
	     */
	    int wait_writers;
	    
	    /* 
	     * counters for wating readers and writers 
	     */
	    int count_wait_readers;
	    int count_wait_writers;
	    
	public:
	    SingleLock(int current, AccessMode mode);	
	    ~SingleLock();    
	    int is_reader_waiting();
	    int is_writer_waiting();	    
	    void setCurrent(TransactionID* current, AccessMode mode);	    
    };
}
#endif

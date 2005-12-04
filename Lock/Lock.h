#ifndef _LOCK_
#define _LOCK_

namespace LockMgr { class LockManager;}

#include "../Store/Store.h"
#include "../Store/DBPhysicalID.h"
#include "../TransactionManager/Transaction.h"
#include "../TransactionManager/Semlib.h"
#include "../Errors/ErrorConsole.h"

#include <deque>
#include <map>
#include <set>
#include <iterator>

using namespace Store;
using namespace TManager;
using namespace Errors;

namespace LockMgr {

    /* class declaration */
//    class SingleLock;

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
	     * transactions that wait for reading an object (wait on a semaphore with unique id = wait_readers)
	     */
	    int wait_readers;
	    
	    /* 
	     * transactions that wait for writing an object (wait on a semaphore with unique id = wait_writers)
	     */
	    int wait_writers;
	    
	    /* 
	     * counters for wating readers and writers 
	     */
	    int count_wait_readers;
	    int count_wait_writers;
	    
	public:
	    SingleLock(TransactionID* tid, AccessMode mode, int current_sem);	
	    ~SingleLock();    
	    int is_reader_waiting();
	    int is_writer_waiting();	    
	    void setCurrent(TransactionID* current, AccessMode mode);	    
    	    int wait_for_lock(TransactionID* tid, AccessMode mode);
    };
    
    /* class to compare DBPhysicalID's */    
    class DBPhysicalIDCmp
    {
        public:
		bool operator() (const DBPhysicalID& , const DBPhysicalID& ) const;
    }; 

    /* class to compare TransactionID's */
    class TransactionIDCmp
    {
        public:
		bool operator() (const TransactionID& , const TransactionID& ) const;
    }; 


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
	     * map from PhysicalId to SingleLock, 
	     */
	    typedef map<Store::DBPhysicalID , SingleLock* , DBPhysicalIDCmp> PhysicalIdMap;
	    PhysicalIdMap* map_of_locks;
	    
	    /* 
	     * map from transactionId to list of PhysicalIds locked 
	     */
	    typedef map<TransactionID , set<DBPhysicalID>*, TransactionIDCmp> TransactionIdMap;
	    TransactionIdMap* transaction_locks;
	    
	    /*
	     * transaction released object 
	     * ( hovewer it can be locked by other tranasactions if share_lock was used )
	     */
	    int unlock(DBPhysicalID, TransactionID*);
	    
	    /* For every DBPhysicalID we need two semaphores: 
	     * 	a. for readers
	     * 	b. for writers
	     * For every semaphore we need a unique number. This number 
	     * is current_sem_key - incremented with every new semaphore
	     */
	    int current_sem_key;
	    int mutex;
	    ErrorConsole err;	    
	    
	    /* sem ids that was created - needed to release all sems in destructor */
	    set<int>* sem_created;
	    
	public: 
	    /*
	     * Returns the only instance of class
	     */
	    static LockManager* getHandle();
	    
	    /*
	     * destruct object - release all semaphores
	     */
	     ~LockManager();
	    /*
	     * if object is already locked, Transaction tid will wait on a semaphore
	     */
	    int lock(LogicalID* , TransactionID* , AccessMode );
	    
	    /*
	     * Transaction calls commit or rollback on TransactionManager, who calls LockManager
	     * to unlock all objects related with this transaction
	     */
	    int unlockAll(TransactionID* );
    };



}
#endif

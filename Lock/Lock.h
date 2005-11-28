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
    
    class LockManager{
	private:
	    static LockManager* lockMgr;
    	    LockManager();
	    /* map from PhysicalId to SingleLock */
	    map<Store::DBPhysicalID* , SingleLock* >* map_of_locks;
	    /* map from transactionId to list of PhysicalIds locked */
	    map<TransactionID* , set<DBPhysicalID*>* >* transaction_locks;
	    int unlock(DBPhysicalID*);
	public: 
	    static LockManager* getHandle();
	    int lock(LogicalID* lid, TransactionID* tid, AccessMode mode);
	    int unlockAll(TransactionID* );
    };

    class SingleLock
    {
	private:
	    /* current ids transaction that locked an object */
	    set<TransactionID*> current; 
	    /* current lock mode: Read | Write */
	    AccessMode current_mode;
	    /* transaction ids that wait for reading an object */
	    int wait_readers;
	    /* transaction ids that wait for writing an object */
	    int wait_writers;
	    /* counters for watinr=g readers and writers */
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

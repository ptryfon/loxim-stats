#ifndef _LOCK_
#define _LOCK_

namespace LockMgr { class LockManager; }

#include "../Store/Store.h"
#include "../Store/DBPhysicalID.h"
#include "../TransactionManager/Transaction.h"

#include <deque>
#include <map>
#include <set>
#include <iterator>

using namespace Store;
using namespace TManager;

namespace LockMgr {

    class SingleLock;
    
    class LockManager{
	private:
	    static LockManager* lockMgr;
    	    LockManager();
	    /* from PhysicalId to structure of lock */
	    map<Store::DBPhysicalID* , SingleLock* >* array_of_locks;
	    /* from transactionId to list of PhysicalIds locked */
	    map<int, set<DBPhysicalID*>* >* transaction_locks;
	    int unlock(DBPhysicalID*);
	public: 
	    static LockManager* getHandle();
	    int lock(LogicalID* lid, TransactionID* tid, AccessMode mode);
	    int unlockAll(int);
    };

    class SingleLock
    {
	private:
	    /* current id transaction that locked an object */
	    int current; 
	    /* current lock mode: Read | Write */
	    AccessMode current_mode;
	    /* transaction ids that wait for reading an object */
	    deque<int>* wait_readers;
	    /* transaction ids that wait for writing an object */
	    deque<int>* wait_writers;
	    
	public:
	    SingleLock(int current, AccessMode mode);
	    void addReader(int reader_id);
	    void addWriter(int writer_id);
	    int getReader();
	    int getWriter();
	    int is_reader();
	    int is_writer();	    
	    void setCurrent(int current, AccessMode mode);	    
    };
}
#endif

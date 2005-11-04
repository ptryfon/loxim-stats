#ifndef _LOCK_
#define _LOCK_

#include "../Store/Store.h"
#include "../TransactionManager/Transaction.h"
using namespace Store;
using namespace TManager;

namespace LockMgr {

class LockManager{
    private:
	    static LockManager* lockMgr;
    	    LockManager();
    public: 
	    static LockManager* getHandle();
	    int lock(LogicalID* lid, TransactionID* tid, AccessMode mode);
};

}
#endif

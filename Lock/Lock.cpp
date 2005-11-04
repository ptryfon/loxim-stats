#include <stdio.h>
#include "Lock.h"

namespace LockMgr{

    LockManager* LockManager::lockMgr = new LockManager();

    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {
	printf("Object locked\n"); fflush(stdout);
        return 0;
    }

    LockManager* LockManager::getHandle() { return lockMgr; }
    
    LockManager::LockManager() {}
}

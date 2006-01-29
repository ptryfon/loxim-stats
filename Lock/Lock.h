#ifndef _LOCK_
#define _LOCK_

namespace LockMgr { class LockManager; }

#include <stdio.h>
#include <stdlib.h>
#include <deque>
#include <map>
#include <set>
#include <iterator>
#include <iomanip>
#include <algorithm>

#include "Comparator.h"
#include "SingleLock.h"
#include "../Store/Store.h"
#include "../Store/DBPhysicalID.h"
#include "../TransactionManager/Transaction.h"
#include "../TransactionManager/SemHeaders.h"
#include "../Errors/ErrorConsole.h"
#include "../Errors/Errors.h"



using namespace Store;
using namespace TManager;
using namespace Errors;
using namespace SemaphoreLib;

namespace LockMgr 
{
    

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
	    	typedef map<Store::DBPhysicalID* , SingleLock* , DBPhysicalIDCmp> DBPhysicalIdMap;
	    	DBPhysicalIdMap* map_of_locks;
	    
	    	/* 
	    	 * map from transactionId to list of PhysicalIds locked 
	    	 */
	    	typedef set<DBPhysicalID*, DBPhysicalIDCmp> DBPhysicalIdSet;
   	    	typedef set<SingleLock*, SingleLockCmp> SingleLockSet;
	    	typedef map<TransactionID* , SingleLockSet*, TransactionIDCmp> TransactionIdMap;
	    	TransactionIdMap* transaction_locks;
	    
	    	/*
	    	 * transaction released object 
	    	 * ( hovewer it can be locked by other tranasactions if share_lock was used )
	    	 */
	    	int unlock(SingleLock* , TransactionID*);
	    
	    	int single_lock_id;
	    	Mutex *mutex;
	    	ErrorConsole err;	    
	    
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

} /* namespace LockMgr */
#endif

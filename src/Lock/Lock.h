#ifndef _LOCK_
#define _LOCK_
/**
 *	@author Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 */
namespace LockMgr { class LockManager; }

#include <stdio.h>
#include <stdlib.h>
#include <deque>
#include <map>
#include <set>
#include <iterator>
#include <iomanip>
#include <algorithm>

#include <Lock/Comparator.h>
#include <Lock/SingleLock.h>
#include <Store/Store.h>
#include <Store/DBPhysicalID.h>
#include <TransactionManager/Transaction.h>
#include <Errors/ErrorConsole.h>
#include <Errors/Errors.h>
#include <Util/Concurrency.h>



using namespace Store;
using namespace TManager;
using namespace Errors;

namespace LockMgr 
{
    

	typedef set<SingleLock*, SingleLockCmp> SingleLockSet;

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
	    	typedef map<TransactionID* , SingleLockSet*, TransactionIDCmp> TransactionIdMap;
	    	TransactionIdMap* transaction_locks;
	    
	    	/*
	    	 * transaction released object 
	    	 * ( hovewer it can be locked by other tranasactions if share_lock was used )
	    	 */
	    	int unlock(SingleLock* , TransactionID*);
	    	int lock_primitive(LogicalID* , TransactionID* , AccessMode, Util::Mutex::Locker &l);
	    
	    	int single_lock_id;
			Util::Mutex *mutex;
	    	ErrorConsole &err;	    
	    
	public: 

	    	/*
	    	 * Returns the only instance of class
	    	 */
	    	static LockManager* getHandle();
	    
	    	/*
	    	 * Initialize LockManager
	    	 */
	    	static void init();
	    	/*
	    	 * destruct object - release all semaphores
	    	 */
	     	~LockManager();
	    	
		/*
	    	 * if object is already locked, Transaction tid will wait on a semaphore
	    	 */
	    	int lock(LogicalID* , TransactionID* , AccessMode );

		/*
	    	 * if object is already locked, Transaction tid will wait on a semaphore
	    	 */
	    	int lockAll(set<LogicalID*>* , TransactionID* , AccessMode );
	    
	    	/*
	    	 * Transaction calls commit or rollback on TransactionManager, who calls LockManager
	    	 * to unlock all objects related with this transaction
	     	*/
	    	int unlockAll(TransactionID* );

	    	/*
		 * object modification is implemented as deleting the old and creating the new one
		 * so we must also modify lock to be seen as old one with new value
	     	*/
	    	int modifyLockLID(DBPhysicalID* , LogicalID* , TransactionID*); 
};

} /* namespace LockMgr */
#endif

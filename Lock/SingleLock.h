#ifndef _SINGLELOCK_
#define _SINGLELOCK_

namespace LockMgr { class SingleLock; };
#include <stdio.h>
#include <stdlib.h>
#include <deque>
#include <map>
#include <set>
#include <iterator>
#include <iomanip>
#include <algorithm>

#include "Comparator.h"
#include "../Store/Store.h"
#include "../Store/DBPhysicalID.h"
#include "../TransactionManager/Transaction.h"
#include "../TransactionManager/SemHeaders.h"
#include "../Errors/ErrorConsole.h"

using namespace Store;
using namespace TManager;
using namespace Errors;
using namespace SemaphoreLib;

namespace LockMgr 
{

/*
 *	Class representing Lock for single Object
 */
class SingleLock
{
	private:
	    
    		friend class SingleLockCmp;
	    	/* 
		 * current transaction ids that locked an object 
		 * (if the mode is for reading there may be many transactions that hold on object )
	     	 */
	    	typedef set<TransactionID ,TransactionIDCmp> TransactionIdSet;
	    	TransactionIdSet* current; 

		/* how many transaction locks an object */
	    	int inside;
	    	
		/* 
	     	 * Transaction id that created this single lock object 
	     	 */
	    	TransactionID *tid;
	    
		/*
		 * returns id - to compare SingleLocks
		 */
		int id;
		int getId() const; 

		/*
		 * Semaphore for exclusion for accessing this object 
		 */
		Semaphore *sem;
		
		/*
		 * Protection if shared data
		 */
		Mutex *mutex;
	public:
	    	SingleLock(TransactionID* tid, AccessMode mode, Semaphore *_sem, int id);	
	    	~SingleLock();     
   	    	int wait_for_lock(TransactionID* tid, AccessMode mode);
   	    	int unlock(TransactionID*);
};


} /* namespace LockMgr */

#endif

#ifndef _SINGLELOCK_
#define _SINGLELOCK_

/**
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 *	@author Julian Krzemiñski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace LockMgr { class SingleLock; };
#include <stdio.h>
#include <stdlib.h>
#include <deque>
#include <map>
#include <set>
#include <iterator>
#include <iomanip>
#include <algorithm>

#include <Lock/Comparator.h>
#include <Store/Store.h>
#include <Store/DBPhysicalID.h>
#include <TransactionManager/Transaction.h>
#include <Util/Concurrency.h>
#include <Errors/ErrorConsole.h>
#include <Errors/Errors.h>

using namespace Store;
using namespace TManager;
using namespace Errors;

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
	    	typedef set<TransactionID* ,TransactionIDCmp> TransactionIdSet;
	    	TransactionIdSet* currentRead;
		TransactionIdSet* currentWrite;

		/* how many transactions lock an object */
	    	int inside;
	    	
		/* how many transactions waiting for an object - and if any - cannot delete the object */
		int waiting;
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
		Util::RWUJSemaphore *sem;
		
		/*
		 * Protection if shared data
		 */
		Util::Mutex *mutex;
		DBPhysicalID *phid;
		
		ErrorConsole &err;

		/* deadlock prevention - wait-die strategy */
		int preventDeadlock(TransactionID* _tid);

	public:
	    	SingleLock(TransactionID* tid, AccessMode mode, Util::RWUJSemaphore *_sem, int id);	
	    	~SingleLock();     
   	    	int wait_for_lock(TransactionID* tid, AccessMode mode);
   	    	int unlock(TransactionID*);
		
		void setPHID(DBPhysicalID *dbid);
		DBPhysicalID* getPHID() const; 
};


} /* namespace LockMgr */

#endif


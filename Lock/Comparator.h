#ifndef _COMPARATOR_
#define _COMPARATOR_

namespace LockMgr { 
	class TransactionIDCmp; 
	class DBPhysicalIDCmp;
	class SingleLockCmp;
}; 

#include "../Store/DBPhysicalID.h"
#include "../TransactionManager/Transaction.h"
#include "SingleLock.h"

using namespace Store;
using namespace TManager;


namespace LockMgr 
{

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

/* class to compare SingleLocks objects */
class SingleLockCmp
{
       	public:
		bool operator() (const SingleLock *, const SingleLock *) const;
}; 

} /* namespace LockMgr */

#endif

#ifndef _COMPARATOR_
#define _COMPARATOR_

/**
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 *	@author Julian Krzemiński (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace LockMgr { 
	class TransactionIDCmp; 
	class DBPhysicalIDCmp;
	class SingleLockCmp;
}; 

#include <Store/DBPhysicalID.h>
#include <TransactionManager/Transaction.h>
#include <Lock/SingleLock.h>

using namespace Store;
using namespace TManager;


namespace LockMgr 
{

/* class to compare DBPhysicalID's */    
class DBPhysicalIDCmp
{
       	public:
		bool operator() (const DBPhysicalID* , const DBPhysicalID* ) const;
}; 

/* class to compare TransactionID's */
class TransactionIDCmp
{
       	public:
		bool operator() (const TransactionID* , const TransactionID* ) const;
}; 

/* class to compare SingleLocks objects */
class SingleLockCmp
{
       	public:
		bool operator() (const SingleLock* , const SingleLock* ) const;
}; 

} /* namespace LockMgr */

#endif

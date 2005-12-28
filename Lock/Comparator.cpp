
#include "Comparator.h"

namespace LockMgr
{

    /* returns true if db1 > db2  */
   bool DBPhysicalIDCmp::operator() (const DBPhysicalID& db1, const DBPhysicalID& db2) const 
   {
		unsigned short f1, f2, p1, p2, o1, o2;
		f1 = db1.getFile();
		f2 = db2.getFile();
	    
		if (f1 != f2) return f1<f2;
		    
		p1 = db1.getPage();
		p2 = db2.getPage();
		    
		if (p1 != p2) return p1<p2;
	    
		o1 = db1.getOffset();
		o2 = db2.getOffset();
	    
		return o1<o2;	
   }

    /* returns true if tid1 > tid2 */
   bool TransactionIDCmp::operator() (const TransactionID& tid1, const TransactionID& tid2) const 
   {
		return ( tid1.getId() > tid2.getId());
   }
   
   /* returns true if sl_1 > sl_2 */
   bool SingleLockCmp::operator() (const SingleLock& sl_1, const SingleLock& sl_2) const 
   {
		return ( sl_1.getId() > sl_2.getId() );
   }
   
} /* namespace LockMgr */


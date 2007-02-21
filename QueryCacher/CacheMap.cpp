
#include "CacheMap.h"

/*
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 */
namespace QCacher
{

    CacheEntry::CacheEntry() {}
    CacheEntry::~CacheEntry() {}
    
    CacheEntry::CacheEntry(TreeNode* t, QueryResult* r)
    {
	result = r;
	query = t;
	locks = new SingleLockSet();
	tids = new vector<int>();	
    }


    /* returns true if db1 > db2  */
   bool TreeNodeCmp::operator() (const TreeNode* tn1, const TreeNode* tn2) const 
   {
	if (tn1 == 0)	return (tn2 == 0);
	if (tn2 == 0)	return (tn1 == 0);
	
	return 0;
	
/*	unsigned short f1, f2, p1, p2, o1, o2;
	f1 = db1->getFile();
	f2 = db2->getFile();
	
	if (f1 != f2) return f1<f2;
		    
	p1 = db1->getPage();
	p2 = db2->getPage();
		    
	if (p1 != p2) return p1<p2;
	    
	o1 = db1->getOffset();
	o2 = db2->getOffset();
	    
	return o1<o2;	*/
   }

} /* namespace QCacher */


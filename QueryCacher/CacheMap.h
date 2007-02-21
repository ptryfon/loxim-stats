#ifndef _CACHE_MAP_
#define _CACHE_MAP_

#include <vector>
#include "../Lock/Lock.h"
#include "../QueryExecutor/QueryResult.h"

using namespace QExecutor;
using namespace LockMgr;

namespace QCacher
{
	class CacheEntry
	{
	    public:
		QueryResult*	result;
		TreeNode*	query;
		SingleLockSet*	locks;
		vector<int>*	tids;

		CacheEntry();
		~CacheEntry();
		CacheEntry(TreeNode*, QueryResult*);
		
	};

	/**
	 *	AbstractCacheMap - an interface to all implementation of hashmap like stores for cached queries
	 */
	class AbstractCacheMap
	{
	    public:
		    AbstractCacheMap();
		    virtual ~AbstractCacheMap();
	    
		    virtual bool get(TreeNode*, CacheEntry*&) = 0;
		    virtual bool put(TreeNode*, CacheEntry*) = 0;

		    virtual bool remove(TreeNode*) = 0;
		    virtual void clear() = 0;
		    virtual void setSize(int) = 0;
		    virtual int getSize(int) = 0;
		    virtual int getElementsCount() = 0;
	};
	
	/* class to compare TreeNodes's */    
	class TreeNodeCmp
	{
       	    public:
		    bool operator() (const TreeNode* , const TreeNode* ) const;
	};

};

#endif

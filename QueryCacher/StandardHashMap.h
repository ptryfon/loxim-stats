#ifndef _CACHE_MAP_
#define _CACHE_MAP_


#include "CacheMap.h"


namespace QCacher
{
	/**
	 *	AbstractCacheMap - an interface to all implementation of hashmap like stores for cached queries
	 */
	class StandardHashMap : public AbstractCacheMap
	{
	    private:
	    	typedef map<TreeNode* , CacheEntry*, TreeNodeCmp> QueryMap;
	    	QueryMap* cache_hashmap;

	    public:
		    StandardHashMap();
		    virtual ~StandardHashMap();
	    
		    virtual bool get(TreeNode*, CacheEntry*&) = 0;
		    virtual bool put(TreeNode*, CacheEntry*) = 0;

		    virtual bool remove(TreeNode*) = 0;
		    virtual void clear() = 0;
		    virtual void setSize(int) = 0;
		    virtual int getSize(int) = 0;
		    virtual int getElementsCount() = 0;
	};

};

#endif

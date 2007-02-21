
#include "QueryCacher.h"

namespace QCacher
{
	StandardHashMap::StandardHashMap() {
	    
	}
	virtual ~StandardHashMap();
	    
	virtual bool get(TreeNode* tn, CacheEntry* &ce) = 0;
	virtual bool put(TreeNode* tn, CacheEntry* &ce) = 0;

	virtual bool remove(TreeNode*) = 0;
	virtual void clear() = 0;
	virtual void setSize(int) = 0;
	virtual int getSize(int) = 0;
	virtual int getElementsCount() = 0;

}

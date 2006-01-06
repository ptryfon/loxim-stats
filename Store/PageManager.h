#ifndef __STORE_PAGEMANAGER_H_
#define __STORE_PAGEMANAGER_H_

#include "Store.h"

namespace Store
{

	class PageManager
	{
	public:
		int binarize(ObjectPointer *obj, unsigned char *buffer, int size);
	private:
	};

};

#endif // __STORE_PAGEMANAGER_H__

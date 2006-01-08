#ifndef __STORE_PAGEMANAGER_H_
#define __STORE_PAGEMANAGER_H_

#include "Store.h"

namespace Store
{

	class PageManager
	{
	public:
		static int binarizeSize(ObjectPointer *obj);
		static int binarize(ObjectPointer *obj, unsigned char **buff);
	};

};

#endif // __STORE_PAGEMANAGER_H__

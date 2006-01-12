#ifndef __STORE_PAGEMANAGER_H_
#define __STORE_PAGEMANAGER_H_

#include "Store.h"
#include "Struct.h"
#include "PagePointer.h"

namespace Store
{

	class PageManager
	{
	public:
		static int binarizeSize(ObjectPointer *obj);
		static int binarize(ObjectPointer *obj, unsigned char **buff);
		static int writeNewHeaders(PagePointer *ptr, page_header hdr, page_header_ex hdx);
		static int writeObject();
	};

};

#endif // __STORE_PAGEMANAGER_H__

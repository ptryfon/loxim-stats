#ifndef __STORE_PAGEMANAGER_H_
#define __STORE_PAGEMANAGER_H_

#include "Store.h"
#include "Struct.h"
#include "PagePointer.h"

namespace Store
{

	typedef struct BinaryObject
	{
		BinaryObject(int size);
		unsigned char *bytes;
		int size;
	} BinaryObject;

	class PageManager
	{
	public:
		static int binarizeSize(ObjectPointer *obj);
		static int binarize(ObjectPointer *obj, BinaryObject*& binobj);
		static int writeNewHeader(PagePointer *ptr, page_header hdr);
		static int insertObject(PagePointer *ptr, BinaryObject* obj);
		static int unbinarize(unsigned char *binobj, ObjectPointer*& newobj);
	};

};

#endif // __STORE_PAGEMANAGER_H__

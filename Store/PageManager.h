#ifndef __STORE_PAGEMANAGER_H_
#define __STORE_PAGEMANAGER_H_

#include "Store.h"
#include "Struct.h"
#include "PagePointer.h"
#include "File.h"

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
//		static int binarizeSize(ObjectPointer *obj);
//		static int binarize(ObjectPointer *obj, BinaryObject*& binobj);
		static int writeNewHeader(PagePointer *ptr);
		static int insertObject(PagePointer *ptr, Serialized& obj);
		static int deserialize(PagePointer *ptr, int objindex, ObjectPointer*& newobj);
		static int initializeFile(File* file);
		static int initializePage(unsigned int page_num, char* page);
		static int getFreePage();
		static int getFreePage(int space);
		static int addToFreeMap(PagePointer *ptr);
	};

};

#endif // __STORE_PAGEMANAGER_H__

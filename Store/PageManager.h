#ifndef __STORE_PAGEMANAGER_H_
#define __STORE_PAGEMANAGER_H_

namespace Store{ class PageManager; }

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
	// note: all methods require aquired PagePointers (ptr->aquire() first!)
	{
	public:
		PageManager(DBStoreManager* store);
//		static int binarizeSize(ObjectPointer *obj);
//		static int binarize(ObjectPointer *obj, BinaryObject*& binobj);
//		static int writeNewHeader(PagePointer *ptr);
		static int insertObject(PagePointer *ptr, Serialized& obj);
		static int deserialize(PagePointer *ptr, int objindex, ObjectPointer*& newobj);
		static int initializeFile(File* file);
		static int initializePage(unsigned int page_num, char* page);
		int getFreePage();
		int getFreePage(int space);
		int updateFreeMap(PagePointer *ptr);
		static void printPage(PagePointer *ptr, int lines=4);
		static void printPage(unsigned char* bytes, int lines);
	private:
		DBStoreManager* store;
		Buffer* buffer;
	};

};

#endif // __STORE_PAGEMANAGER_H__

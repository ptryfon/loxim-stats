#ifndef __STORE_PAGEMANAGER_H_
#define __STORE_PAGEMANAGER_H_

namespace Store{ class PageManager; }

#include <Store/Store.h>
#include <Store/Struct.h>
#include <Store/PagePointer.h>
#include <Store/File.h>

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
		static int insertObject(TransactionID* tid, PagePointer *ptr, Serialized& obj, int* pidoffset, unsigned log_id);
		static int deserialize(TransactionID* tid, PagePointer *ptr, int objindex, ObjectPointer*& newobj);
		static int initializeFile(File* file);
		static int initializePage(unsigned int page_num, char* page);
		int getFreePage(TransactionID* tid);
		int getFreePage(TransactionID* tid, int objsize);
		int updateFreeMap(TransactionID* tid, PagePointer *ptr);
		static void printPage(PagePointer *ptr, int lines=4);
		static void printPage(unsigned char* bytes, int lines);
	private:
		DBStoreManager* store;
		Buffer* buffer;
	};

};

#endif // __STORE_PAGEMANAGER_H__

#include "PageManager.h"
/*
  Obiekty na stronie sa posortowane przeciwnie wg offsetu.
  PAGE = <HEADER><EX><OFFSETTABLE><FREESPACE><OBJECTS>
  OFFSETTABLE = tablica mowiaca, gdzie zaczyna sie obiekt o indeksie i
  OBJECTS = obiekty ukladane sa od konca, sa posortowane od najmlodszego
  do najstarszego
*/
#include <iostream>

using namespace std;

namespace Store
{
/*
	BinaryObject::BinaryObject(int psize)
	{
		bytes = new unsigned char[psize];
		size = psize; 
	}

	int PageManager::binarizeSize(ObjectPointer *obj)
	{
		return 0;
	}
	
	int PageManager::binarize(ObjectPointer *obj, BinaryObject*& binobj)
	{
		binobj = new BinaryObject(binarizeSize(obj));
		int writePosition = 0;
		int asize = 0;
		unsigned char* adata = NULL;
		unsigned int tmp = 0;
		
		//LogicalID
		obj->getLogicalID()->toByteArray(&adata, &asize);
		for(int i=0; i<asize; i++)
			binobj->bytes[writePosition++] = adata[i];
		if(adata) delete[] adata;
		adata = NULL;
		
		//random
		tmp = (rand()%0x100) << 24 + (rand()%0x100) << 16 +
			(rand()%0x100) << 8 + (rand()%0x100);
		adata = reinterpret_cast<unsigned char*>(&tmp);
		for(unsigned int i=0; i<sizeof(unsigned int); i++)
			binobj->bytes[writePosition++] = adata[i];
		adata = NULL;
		
		string name = obj->getName();
		//namesize
		tmp = static_cast<unsigned int>(name.length());
		adata = reinterpret_cast<unsigned char*>(&tmp);
		for(unsigned int i=0; i<sizeof(unsigned int); i++)
			binobj->bytes[writePosition++] = adata[i];
		adata = NULL;

		//name
		asize = name.length();
		for(int i=0; i<asize; i++)
			binobj->bytes[writePosition++] =
				static_cast<unsigned char>(name[i]);
		
		//value
		obj->getValue()->toFullByteArray(&adata, &asize);
		for(int i=0; i<asize; i++)
			binobj->bytes[writePosition++] = adata[i];
		if(adata) delete[] adata;
		adata = NULL;

	
		return 0;
	}
*/	
	int PageManager::writeNewHeader(PagePointer *pPtr)
	{
		page_data *page = reinterpret_cast<page_data*>(pPtr->getPage());
		
		page->header.file_id = pPtr->getFileID();
		page->header.page_id = pPtr->getPageID();
		page->header.page_type = 0;
		page->header.timestamp = 0;
		page->object_count = 0;
		page->free_space = STORE_PAGESIZE - sizeof(page_header);
		
		return 0;
	}
	
	int PageManager::insertObject(PagePointer *pPtr, Serialized& obj)
	{
		page_data *page = reinterpret_cast<page_data*>(pPtr->getPage());
		
		if(page->free_space < static_cast<int>(obj.size + sizeof(int))){
			cout << "Store::PageManager::insertObject not enough free space on page\n";
			return -1;
		}
		int lastoffset = page->object_count > 0 ?
			page->object_offset[page->object_count-1] : STORE_PAGESIZE;
		page->free_space -= obj.size + sizeof(int);
		int newoffset = lastoffset - obj.size - sizeof(int);
		page->object_offset[page->object_count] = newoffset;
		for(int i=0; i<obj.size; i++)
			page->bytes[newoffset+i] = obj.bytes[i];
		page->object_count++;

		return 0;
	}
	
	int PageManager::deserialize(PagePointer *ptr, int objindex, ObjectPointer*& newobj)
	{
	
		return 0;
	}
	
}

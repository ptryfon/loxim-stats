#include "PageManager.h"
/*
  Obiekty na stronie sa posortowane przeciwnie wg offsetu.
  PAGE = <HEADER><EX><OFFSETTABLE><FREESPACE><OBJECTS>
  OFFSETTABLE = tablica mowiaca, gdzie zaczyna sie obiekt o indeksie i
  OBJECTS = obiekty ukladane sa od konca, sa posortowane od najmlodszego
  do najstarszego
*/
#include <iostream>
#include "DBLogicalID.h"
#include "DBObjectPointer.h"
#include "DBDataValue.h"

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
		page_data *p = reinterpret_cast<page_data*>(ptr->getPage());
		int osize = objindex > 0 ?
			p->object_offset[objindex-1] - p->object_offset[objindex] :
			STORE_PAGESIZE - p->object_offset[objindex];
		unsigned char *startpos =
			reinterpret_cast<unsigned char*>(p->bytes[p->object_offset[objindex]]);
		int usedbytes;
		unsigned char *curpos = startpos;

		DBLogicalID *lid;
		usedbytes = DBLogicalID::deserialize(curpos, lid);
		if(usedbytes > 0 ) curpos = curpos + usedbytes; else return -1;
		
		int slen = *(reinterpret_cast<int*>(curpos));
		curpos += sizeof(int);
		string name;
		for(int i=0; i<slen; i++)
			name += *(reinterpret_cast<char*>(curpos++));
		
		DBDataValue *value;
		usedbytes = DBDataValue::deserialize(curpos, value);
		if(usedbytes > 0 ) curpos = curpos + usedbytes; else return -1;		
		
		if(curpos-startpos > osize) return -1;
		
		newobj = new DBObjectPointer(name, value, lid);
		
		return 0;
	}
	
	int PageManager::initializeFile(File* file)
	{
	
		return 0;
	}

	int PageManager::initializePage(unsigned int page_num, char* page)
	{
		page_data *p = reinterpret_cast<page_data*>(page);
		
		p->header.file_id = STORE_FILE_DEFAULT;
		p->header.page_id = page_num;
		p->header.page_type = STORE_PAGE_DATAHEADER; // nie istotne
		p->header.timestamp = 0;
		p->object_count = 0;
		p->free_space = STORE_PAGESIZE - sizeof(page_header);
		
		return 0;
	}
	
	int PageManager::getFreePage()
	{
		return 0;
	}

	int PageManager::getFreePage(int space)
	{
		return 0;
	}

	int PageManager::addToFreeMap(PagePointer *pPtr)
	{
		return 0;
	}
	
}

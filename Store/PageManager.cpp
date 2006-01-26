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
#include "Errors/ErrorConsole.h"

#define MAX_FREE_SPACE   (STORE_PAGESIZE-sizeof(page_data))
#define MAX_OBJECT_SIZE  (STORE_PAGESIZE-sizeof(page_data)-sizeof(int))
#define MAX_OBJECT_COUNT (MAX_FREE_SPACE/sizeof(int))

using namespace std;

namespace Store
{

	PageManager::PageManager(DBStoreManager* p_store)
	{
		store = p_store;
		buffer = store->getBuffer();
	}
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
/*	int PageManager::writeNewHeader(PagePointer *pPtr)
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
*/	
	int PageManager::insertObject(PagePointer *pPtr, Serialized& obj, int* pidoffset, unsigned log_id)
	{
		ErrorConsole ec("Store");
		ec << "Store::PageManager::insertObject begin...";
		page_data *page = reinterpret_cast<page_data*>(pPtr->getPage());
		
		if(page->free_space < static_cast<int>(obj.size + sizeof(int))){
			ec << "Store::PageManager::insertObject not enough free space on page";
			return -1;
		}
		int lastoffset = page->object_count > 0 ?
			page->object_offset[page->object_count-1] : STORE_PAGESIZE;
		page->free_space -= obj.size + sizeof(int);
		int newoffset = lastoffset - obj.size;
		page->object_offset[page->object_count] = newoffset;
		*pidoffset = page->object_count;
		for(int i=0; i<obj.size; i++)
			page->bytes[newoffset+i] = obj.bytes[i];
		page->object_count++;
		
		page->header.timestamp = static_cast<int>(log_id);
		
		ec << "Store::PageManager::insertObject done";
		return 0;
	}
	
	int PageManager::deserialize(PagePointer *ptr, int objindex, ObjectPointer*& newobj)
	{
		ErrorConsole ec("Store");
		ec << "Store::PageManager::deserializeObj begin...";
		page_data *p = reinterpret_cast<page_data*>(ptr->getPage());
		int osize = objindex > 0 ?
			p->object_offset[objindex-1] - p->object_offset[objindex] :
			STORE_PAGESIZE - p->object_offset[objindex];
		unsigned char *startpos =
			reinterpret_cast<unsigned char*>(&(p->bytes[p->object_offset[objindex]]));
		int usedbytes;
		unsigned char *curpos = startpos;

		DBLogicalID *lid;
		usedbytes = DBLogicalID::deserialize(curpos, lid);
		if(usedbytes > 0 ) curpos = curpos + usedbytes; else return -1;
		
		int random = *(reinterpret_cast<int*>(curpos));
		curpos += sizeof(int);
		
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
		
		ec << "Store::PageManager::deserializeObj done";
		return 0;
	}
	
	int PageManager::initializeFile(File* file)
	{
		ErrorConsole ec("Store");
		ec << "Store::PageManager::initializeFile begin...";
		char* rawpage = new char[STORE_PAGESIZE];
		page_data* p = reinterpret_cast<page_data*>(rawpage);
		
		memset(rawpage, 0, STORE_PAGESIZE);
		initializePage(0, rawpage);
		p->header.page_type = STORE_PAGE_DATAHEADER;
		p->object_count = 0;
		p->free_space = MAX_FREE_SPACE / sizeof(int);
		
		file->writePage(STORE_FILE_DEFAULT, 0, rawpage);
		
		delete[] rawpage;
		ec << "Store::PageManager::initializeFile done";
		return 0;
	}

	int PageManager::initializePage(unsigned int page_num, char* page)
	{
		ErrorConsole ec("Store");
		ec << "Store::PageManager::initializePage begin...";
		bool isFreeMapPage = (page_num%(MAX_OBJECT_COUNT+1) == 0);
	
		page_data *p = reinterpret_cast<page_data*>(page);
		
		p->header.file_id = STORE_FILE_DEFAULT;
		p->header.page_id = page_num;
		p->header.timestamp = 0;
		p->object_count = 0;

		if(isFreeMapPage) {
			p->header.page_type = STORE_PAGE_DATAHEADER;
			p->free_space = MAX_OBJECT_COUNT;
		} else {
			p->header.page_type = STORE_PAGE_DATAPAGE;
			p->free_space = MAX_FREE_SPACE;
		}
		ec << "Store::PageManager::initializePage done";
		return 0;
	}

	int PageManager::getFreePage()
	{
		return getFreePage(MAX_FREE_SPACE);
	}
	
	int PageManager::getFreePage(int space)
	{
		ErrorConsole ec("Store");
		ec << "Store::PageManager::getFreePage begin...";
		int pii = 0;
		do	{
			PagePointer* pPtr = buffer->getPagePointer(STORE_FILE_DEFAULT, pii);
			pPtr->aquire();
			page_data* p = reinterpret_cast<page_data*>(pPtr->getPage());
			
			for(int i=0; i<p->object_count; i++) {
				if(p->object_offset[i] >= space) {
					int rid = pPtr->getPageID()+i+1;
					pPtr->release();
					ec << "Store::PageManager::getFreePage done";
					return (rid);
				}
			}
			if(p->object_count < static_cast<int>(MAX_OBJECT_COUNT)) {
				int rid = pPtr->getPageID()+p->object_count+1;
				pPtr->release();
				ec << "Store::PageManager::getFreePage done";
				return (rid);
			}
			
			pPtr->release();
			pii += MAX_OBJECT_COUNT+1;
		} while(pii > 0);
		
		ec << "Store::PageManager::getFreePage done";
		return 0;
	}

	int PageManager::updateFreeMap(PagePointer *pPtr)
	{
		ErrorConsole ec("Store");
		ec << "Store::PageManager::updateFreeMap begin...";
		page_data* p = reinterpret_cast<page_data*>(pPtr->getPage());

		int pii = p->header.page_id - (p->header.page_id%MAX_OBJECT_COUNT);

		PagePointer* pFree = buffer->getPagePointer(STORE_FILE_DEFAULT, pii);
		pFree->aquire();
		page_data* f = reinterpret_cast<page_data*>(pFree->getPage());

		int offset = (p->header.page_id%MAX_OBJECT_COUNT)-1;
		if(f->object_count < offset) {
			ec << "Store::PageManager::updateFreeMap ERROR: HEADER PAGE FAULT";
			return -1;
		}
		if(f->object_count == offset) {
			f->object_count++;
			f->free_space--;
		}
		f->object_offset[offset] =	p->free_space;
		
		pFree->release();

		ec << "Store::PageManager::updateFreeMap done";
		return 0;
	}
	
	void PageManager::printPage(PagePointer* ptr, int lines)
	{
		unsigned char* bytes = reinterpret_cast<unsigned char*>(ptr->getPage());
		return printPage(bytes, lines);
	}

	void PageManager::printPage(unsigned char* bytes, int lines)
	{
		// DOBRA INTERPRETACJA ALE wypisuje jakies teksty:
		// "Store: create string wit null pointercreate string with null pointercreate...." itd
		//ErrorConsole ec("Store");
		//string tmpstr = "";
		
		for(int l=0; l<lines; l++) {
			//tmpstr = "";
			for(int i=0;i<16;i++) {
				if((i!=0) && (i%4==0))
					//tmpstr += "| ";
					cout << "| ";
					unsigned char AX = bytes[l*16+i];
					char AL = (AX%16>9 ? AX%16-10+'A' : AX%16+'0');
					AX /= 16;
					char AH = (AX%16>9 ? AX%16-10+'A' : AX%16+'0');
				//tmpstr += AH + AL + " ";
				cout << AH << AL << " ";
			}
			//tmpstr += " ";
			cout << " ";
			for(int i=0;i<16;i++) {
				if(bytes[l*16+i] > 31)
					//tmpstr += bytes[l*16+i];
					cout << bytes[l*16+i];
				else
					//tmpstr += ".";
					cout << ".";
			}
			//ec << tmpstr;
			cout << endl;
		}
	
	}
	
}

#include <Store/PageManager.h>
/*
  Obiekty na stronie sa posortowane przeciwnie wg offsetu.
  PAGE = <HEADER><EX><OFFSETTABLE><FREESPACE><OBJECTS>
  OFFSETTABLE = tablica mowiaca, gdzie zaczyna sie obiekt o indeksie i
  OBJECTS = obiekty ukladane sa od konca, sa posortowane od najmlodszego
  do najstarszego tzn offsety 0 1 2 odpowiadaja obiektom <2.1.0.KONIECSTRONY>
*/
#include <iostream>
    #include <Store/DBLogicalID.h>
#include <Store/DBObjectPointer.h>
#include <Store/DBDataValue.h>
#include <Errors/ErrorConsole.h>

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

	int PageManager::insertObject(TransactionID* tid, PagePointer *pPtr, Serialized& obj, int* pidoffset, unsigned log_id)
	{
		ErrorConsole &ec(ErrorConsole::get_instance(EC_STORE));
		debug_print(ec,  "Store::PageManager::insertObject begin...");
		page_data *page = reinterpret_cast<page_data*>(pPtr->getPage());
		int roffset = -1;

		if( page->free_space < static_cast<int>(obj.size) ) {
			debug_print(ec,  "Store::PageManager::insertObject not enough free space on page");
			return -1;
		}

		// przeszukac tablice offsetow czy jest pusty obiekt
		// pusty obiekt to taki obiekt, ktorego poprzednik posiada wskaznik
		// taki sam jak pusty obiekt
		// wartosc wskaznika dla poprzednika obiektu zerowego jest STORE_PAGESIZE
		if( page->object_count > 0 ) {
			// sprawdzenie zerowego obiektu
			if( page->object_offset[0] == STORE_PAGESIZE )
				roffset = 0;
			// sprawdzanie pozostalych
			else
				for(int i=0; i<page->object_count-1; i++) {
					if( page->object_offset[i] <= page->object_offset[i+1] ) {
						if( page->object_offset[i] < page->object_offset[i+1] ) {
							debug_print(ec,  "Store::PageManager::insertObject corrupted page offsets detected");
							return -11;
						}
						else {
							roffset = i+1;
							break;
						}
					}
				}
		}

		if( page->free_space < static_cast<int>(obj.size + sizeof(int)) )
			if( roffset == -1 ) {
				debug_print(ec,  "Store::PageManager::insertObject not enough free space on page");
				return -2;
			}

		if( roffset == -1 ) {
		// dokladamy nowy obiekt na koncu
			int lastoffset = page->object_count > 0 ?
				page->object_offset[page->object_count-1] : STORE_PAGESIZE;
			page->free_space -= obj.size + sizeof(int);
			int newoffset = lastoffset - obj.size;
			page->object_offset[page->object_count] = newoffset;
			*pidoffset = page->object_count;
			for(int i=0; i<obj.size; i++)
				page->bytes[newoffset+i] = obj.bytes[i];
			page->object_count++;
		}
		else {
		// wstawiamy nowy obiekt w miejsce wyszukanego obiektu pustego
		// nalezy przesunac wszystkie obiekty wieksze od pustego
			int i;
			for(i = page->object_offset[page->object_count-1];
				i < page->object_offset[roffset]; i++)
				page->bytes[i-obj.size] = page->bytes[i];
		// dopisac nowy obiekt
			i-= obj.size;
			for(int j=0; j<obj.size; j++)
				page->bytes[i+j] = obj.bytes[j];
		// od wskaznikow odjac rozmiar nowego obiektu
			for(i = roffset; i<page->object_count; i++)
				page->object_offset[i]-= obj.size;
		// zmniejszyc free_space
			page->free_space-= obj.size;
		// ustawic pidoffset dla mapy
			*pidoffset = roffset;
		}

		page->header.timestamp = static_cast<int>(log_id);

		debug_print(ec,  "Store::PageManager::insertObject done");
		return 0;
	}

	int PageManager::deserialize(TransactionID* tid, PagePointer *ptr, int objindex, ObjectPointer*& newobj)
	{
		ErrorConsole &ec(ErrorConsole::get_instance(EC_STORE));
		debug_print(ec,  "Store::PageManager::deserializeObj begin...");
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

		int isRoot = *(reinterpret_cast<int*>(curpos));
		curpos += sizeof(int);

		int random = *(reinterpret_cast<int*>(curpos));
		random = random;	// Warning suspenser
		curpos += sizeof(int);

		int slen = *(reinterpret_cast<int*>(curpos));
		curpos += sizeof(int);
		
		string name((reinterpret_cast<char*>(curpos)), slen);
		curpos+=slen;

		DBDataValue *value;
		usedbytes = DBDataValue::deserialize(tid, curpos, value, true);
		if(usedbytes > 0 ) curpos = curpos + usedbytes; else return -1;

		if(curpos-startpos > osize) return -1;
                //gtimoszuk
                //what is currently newobject

		newobj = new DBObjectPointer(name, value, lid);
		newobj->setIsRoot(isRoot == 1);

                //gtimoszuk
                if (value != NULL) {
                    delete value;
                }
                
                if (lid != NULL) {
                    delete lid;
                }
                
		debug_print(ec,  "Store::PageManager::deserializeObj done");
		return 0;
	}

	int PageManager::initializeFile(File* file)
	{
		ErrorConsole &ec(ErrorConsole::get_instance(EC_STORE));
		debug_print(ec,  "Store::PageManager::initializeFile begin...");
		char* rawpage = new char[STORE_PAGESIZE];
		page_data* p = reinterpret_cast<page_data*>(rawpage);

		memset(rawpage, 0, STORE_PAGESIZE);
		initializePage(0, rawpage);
		p->header.page_type = STORE_PAGE_DATAHEADER;
		p->object_count = 0;
		p->free_space = MAX_FREE_SPACE / sizeof(int);

		file->writePage(STORE_FILE_DEFAULT, 0, rawpage);

		delete[] rawpage;
		debug_print(ec,  "Store::PageManager::initializeFile done");
		return 0;
	}

	int PageManager::initializePage(unsigned int page_num, char* page)
	{
		ErrorConsole &ec(ErrorConsole::get_instance(EC_STORE));
		debug_print(ec,  "Store::PageManager::initializePage begin...");

		bool isFreeMapPage = (page_num%(MAX_OBJECT_COUNT+1) == 0);

		memset(page, 0, STORE_PAGESIZE);
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
		debug_print(ec,  "Store::PageManager::initializePage done");
		return 0;
	}

	int PageManager::getFreePage(TransactionID* tid)
	{
		return getFreePage(tid, MAX_FREE_SPACE-sizeof(int));
	}

	int PageManager::getFreePage(TransactionID* tid, int objsize)
	{
	// objsize to rozmiar obiektu jaki StoreManager chce umiescic na stronie
	// w ogolnosci trzeba wyszukac strone ktora bedzie miala dodatkowo
	// sizeof(int) miejsca, aby dodaj pole do tablicy offsetow
		ErrorConsole &ec(ErrorConsole::get_instance(EC_STORE));
		debug_printf(ec, "Store::PageManager::getFreePage(%i) begin...\n", objsize);
		int pii = 0;
		do	{
			PagePointer* pPtr = buffer->getPagePointer(tid, STORE_FILE_DEFAULT, pii);
			pPtr->aquire(tid);
			page_data* p = reinterpret_cast<page_data*>(pPtr->getPage());

			for(int i=0; i<p->object_count; i++) {
				if( p->object_offset[i] >= static_cast<int>(objsize+sizeof(int)) ) {
					int rid = pPtr->getPageID()+i+1;
					pPtr->release(tid, 0);
					debug_print(ec,  "Store::PageManager::getFreePage done");
					return (rid);
				}
			}
			if(p->object_count < static_cast<int>(MAX_OBJECT_COUNT)) {
				int rid = pPtr->getPageID()+p->object_count+1;
				pPtr->release(tid, 0);
				debug_print(ec,  "Store::PageManager::getFreePage done");
				return (rid);
			}

			pPtr->release(tid, 0);
			pii += MAX_OBJECT_COUNT+1;
		} while(pii > 0);

		debug_print(ec,  "Store::PageManager::getFreePage done");
		return 0;
	}

	int PageManager::updateFreeMap(TransactionID* tid, PagePointer *pPtr)
	{
		ErrorConsole &ec(ErrorConsole::get_instance(EC_STORE));
		debug_print(ec,  "Store::PageManager::updateFreeMap begin...");
		page_data* p = reinterpret_cast<page_data*>(pPtr->getPage());

		int pii = p->header.page_id - (p->header.page_id % (MAX_OBJECT_COUNT + 1));

		PagePointer* pFree = buffer->getPagePointer(tid, STORE_FILE_DEFAULT, pii);
		pFree->aquire(tid);
		page_data* f = reinterpret_cast<page_data*>(pFree->getPage());

		int offset = (p->header.page_id % (MAX_OBJECT_COUNT + 1)) - 1;

		debug_printf(ec, "p->header.page_id = %i, pii = %i, offset = %i\n", p->header.page_id, pii, offset);

		if(f->object_count < offset) {
			debug_print(ec,  "Store::PageManager::updateFreeMap ERROR: HEADER PAGE FAULT");
			return -1;
		}
		if(f->object_count == offset) {
			f->object_count++;
			f->free_space--;
		}
		f->object_offset[offset] =	p->free_space;

		pFree->releaseSync(tid, 1);

		debug_print(ec,  "Store::PageManager::updateFreeMap done");
		return 0;
	}

	void PageManager::printPage(PagePointer* ptr, int lines)
	{
		unsigned char* bytes = reinterpret_cast<unsigned char*>(ptr->getPage());
		return printPage(bytes, lines);
	}

	void PageManager::printPage(unsigned char* bytes, int lines)
	{
		ErrorConsole &ec(ErrorConsole::get_instance(EC_STORE));
		debug_print(ec,  "Store::PageManager::printPage...");
		ostringstream ostr;
		ostr << endl;
		for(int l=0; l<lines; l++) {
			for(int i=0;i<16;i++) {
				if((i!=0) && (i%4==0))
					ostr << "| ";
					unsigned char AX = bytes[l*16+i];
					char AL = (AX%16>9 ? AX%16-10+'A' : AX%16+'0');
					AX /= 16;
					char AH = (AX%16>9 ? AX%16-10+'A' : AX%16+'0');
				ostr << AH << AL << " ";
			}
			ostr << " ";
			for(int i=0;i<16;i++) {
				if(bytes[l*16+i] > 31)
					ostr << bytes[l*16+i];
				else
					ostr << ".";
			}
			ostr << endl;
		}
		debug_print(ec,  ostr.str());
	}

}

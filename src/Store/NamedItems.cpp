/*
 * Ten plik nie jest używany.
 */

#include <Store/NamedItems.h>
#include <Store/File.h>
#include <Store/PagePointer.h>

namespace Store
{
	NamedItems::NamedItems()
	{
#ifdef IX_DEBUG
		this->ec = &ErrorConsole::get_instance(EC_STORE_NAMED_ITEMS);
#endif
	};

	NamedItems::~NamedItems()
	{
#ifdef IX_DEBUG
#endif
	};

	void NamedItems::init(Buffer* buffer, LogManager* log)
	{
		this->buffer = buffer;
		this->log = log;
	}

	int NamedItems::initializeFile(File* file)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "initializeFile()\n");
#endif

		char* buf = new char[STORE_PAGESIZE];
		ixr_header* header = (ixr_header*) buf;
		ixr_page* page = (ixr_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		header->page_hdr.page_id = 0;
		header->page_hdr.file_id = STORE_FILE_;
		header->page_hdr.page_type = STORE_PAGE_HEADER;
		header->page_hdr.timestamp = 0;
		file->writePage(STORE_FILE_, 0, buf);

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_;
		page->page_hdr.page_type = STORE_PAGE_PAGE;
		page->free = STORE_PAGESIZE - sizeof(ixr_page);
		page->entries = 0;

		for (unsigned int i = 1; i < STORE_IX_INITIALPAGECOUNT; i++)
		{
			page->page_hdr.page_id = i;
			page->page_hdr.timestamp = i == 1 ? 0 : STORE_IX_NULLVALUE;
			file->writePage(STORE_FILE_, i, buf);
		}

		delete buf;

		return 0;
	};

	int NamedItems::initializePage(unsigned int pageID, char* buf)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "initializePage(pageID=%i)\n", pageID);
#endif

		ixr_page* page = (ixr_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_;
		page->page_hdr.page_type = STORE_PAGE_PAGE;
		page->page_hdr.timestamp = STORE_IX_NULLVALUE;
		page->page_hdr.page_id = pageID;
		page->free = STORE_PAGESIZE - sizeof(ixr_page);

		return 0;
	}

	int NamedItems::addItem(TransactionID* tid, int size_needed, char* entry_buf) {

		int i = 1;
		//ec->printf("NI::addItem(2) starts\n");
		while (i > 0)
		{
			if (buffer==NULL)
			    debug_printf(*ec, "NamedItems::addItem ERROR!-null buffer (no init called)!\n");
			//ec->printf("NI::aI Store_file_ %d\n", STORE_FILE_);
			PagePointer* page_pointer = buffer->getPagePointer(tid, STORE_FILE_, (unsigned int) i);
			page_pointer->aquire(tid);
			//ec->printf("NI::aI page_pointer aquired\n");

			char* page_buf = page_pointer->getPage();
			ixr_page* page = (ixr_page*) page_buf;

			if (page->free >= size_needed)
			{
				memcpy(page_buf + (STORE_PAGESIZE - page->free), entry_buf, size_needed);
				page->free -= size_needed;
				page->entries++;
				page->page_hdr.timestamp = log->getLogicalTimerValue();

				i = 0;
			}
			else
				i++;

			if (i == 0)
				page_pointer->releaseSync(tid, 1);
			else
				page_pointer->release(tid, 0);

			delete page_pointer;
		}

		return 0;

	}

	int NamedItems::createEntry(TransactionID* tid, int logicalID, const char* name, int& size_needed, char*& entry_buf) {

		int name_len = 0;
		ix_entry* entry = 0;

		//ec->printf("debug1, maxlen = %d, len = %d \n", STORE_IX_NAMEMAXLEN, strlen(name));

		name_len = strlen(name);
		if (name_len > STORE_IX_NAMEMAXLEN)
			return 1;

		size_needed = sizeof(ix_entry) + name_len + 1;
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);

		entry = (ix_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = tid->getId();
		entry->add_t = tid->getTimeStamp();
		entry->del_t = STORE_IX_NULLVALUE;
		memcpy(entry->name, name, name_len);

		//ec->printf("NI::createEntry entry Created!\n");

		return 0;
	}

	int NamedItems::addItem(TransactionID* tid, int logicalID, const char* name)
	{

#ifdef IX_DEBUG
		debug_printf(*ec, "addItem(logicalID=%i, name=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", logicalID, name, tid->getId(), tid->getTimeStamp());
#endif
		char* entry_buf;
		int size_needed = 0;
		//ec->printf("NamedItems::addItem before create Entry\n");
		int errcode = createEntry(tid, logicalID, name, size_needed, entry_buf);
		//ec->printf("NamedItems::addItem after create Entry, errorcode = %d, size_needed = %d \n", errcode, size_needed);
		if(errcode != 0) return errcode;
		if (entry_buf == NULL)
		    debug_printf(*ec, "NamedItems::addItem NULL entry_buf\n");
		addItem(tid, size_needed, entry_buf);
		//ec->printf("NamedItems::addItem past addItem(size, buf)\n");
		if(entry_buf != NULL){
			delete entry_buf;
		}
		return 0;

		/* old version */
		/*
		int i = 0;
		int name_len = 0;
		int size_needed = 0;
		char* entry_buf = 0;
		ixr_entry* entry = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;

		name_len = strlen(name);
		if (name_len > STORE_IX_NAMEMAXLEN)
			return 1;

		size_needed = sizeof(ixr_entry) + name_len + 1;
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);

		entry = (ixr_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = transactionID;
		entry->add_t = transactionTimeStamp;
		entry->del_t = STORE_IX_NULLVALUE;
		memcpy(entry->name, name, name_len);

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(STORE_FILE_, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->free >= size_needed)
			{
				memcpy(page_buf + (STORE_PAGESIZE - page->free), entry_buf, size_needed);
				page->free -= size_needed;
				page->entries++;
				page->page_hdr.timestamp = log->getLogicalTimerValue();

				i = 0;
			}
			else
				i++;

			if (i == 0)
				page_pointer->releaseSync(1);
			else
				page_pointer->release(0);

			delete page_pointer;
		}

		delete entry_buf;

		return 0;
		*/
	};

	int NamedItems::removeItem(TransactionID* tid, int logicalID)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "removeItem(logicalID=%i, transactionID=%i, transactionTimeStamp=%i)\n", logicalID, tid->getId(), tid->getTimeStamp());
#endif

		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;
		ix_entry* entry = 0;
		int entry_size = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(tid, STORE_FILE_, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IX_NULLVALUE)
				i = 0;
			else if (page->entries == 0)
				i++;
			else
			{
				offset = sizeof(ixr_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (ix_entry*) (page_buf + offset);

					if (entry->l_id == logicalID)
					{
						if (entry->cur_tran == STORE_IX_NULLVALUE)
						{
							entry->cur_tran = tid->getId();
							entry->del_t = tid->getTimeStamp();
						}
						else
						{
							entry_size = entry->size;
							memmove(page_buf + offset, page_buf + offset + entry_size, STORE_PAGESIZE - (offset + entry_size + page->free));
							page->free += entry_size;
							page->entries--;
							memset(page_buf + (STORE_PAGESIZE - page->free), 0, page->free);
						}

						page->page_hdr.timestamp = log->getLogicalTimerValue();
						offset = STORE_PAGESIZE + 1;
						i = 0;
					}
					else
						offset += entry->size;
				}

				if (i > 0)
						i++;
			}

			if (i == 0)
				page_pointer->releaseSync(tid, 1);
			else
				page_pointer->release(tid, 0);

			delete page_pointer;
		}

		return 0;
	};

	int NamedItems::commitTransaction(TransactionID* tid)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "commitTransaction(transactionID=%i)\n", tid->getId());
#endif

		return modifyTransaction(tid, 0);
	}

	int NamedItems::abortTransaction(TransactionID* tid)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "abortTransaction(transactionID=%i)\n", tid->getId());
#endif

		return modifyTransaction(tid, 1);
	}

	int NamedItems::abortAllTransactions()
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "abortAllTransactions()\n");
#endif

		return modifyTransaction(0, 2);
	}

	int NamedItems::modifyTransaction(TransactionID* tid, int mode)
	{
		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;
		ix_entry* entry = 0;
		int entry_size = 0;
		int changed = 0;

		i = 1;
		while (i > 0)
		{
			changed = 0;
			page_pointer = buffer->getPagePointer(tid, STORE_FILE_, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IX_NULLVALUE)
				i = 0;
			else if (page->entries == 0)
				i++;
			else
			{
				offset = sizeof(ixr_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (ix_entry*) (page_buf + offset);
					entry_size = entry->size;

					if (entry->cur_tran == tid->getId() || (mode == 2 && entry->cur_tran != STORE_IX_NULLVALUE))
					{
						// Commiting remove
						if (entry->del_t != STORE_IX_NULLVALUE && mode == 0)
						{
							entry->del_t = log->getLogicalTimerValue();
							entry->cur_tran = STORE_IX_NULLVALUE;
						}
						// Commiting add
						else if (entry->del_t == STORE_IX_NULLVALUE && mode == 0)
						{
							entry->add_t = log->getLogicalTimerValue();
							entry->cur_tran = STORE_IX_NULLVALUE;
						}
						// Aborting remove
						else if (entry->del_t != STORE_IX_NULLVALUE && mode > 0)
						{
							entry->del_t = STORE_IX_NULLVALUE;
							entry->cur_tran = STORE_IX_NULLVALUE;
						}
						// Aborting add
						else if (entry->del_t == STORE_IX_NULLVALUE && mode > 0)
						{
							memmove(page_buf + offset, page_buf + offset + entry_size, STORE_PAGESIZE - (offset + entry_size + page->free));
							page->free += entry_size;
							page->entries--;
							memset(page_buf + (STORE_PAGESIZE - page->free), 0, page->free);
							entry_size = 0;
						}

						changed = 1;
						page->page_hdr.timestamp = log->getLogicalTimerValue();
					}

					offset += entry_size;
				}

				i++;
			}

			if (changed != 0)
				page_pointer->releaseSync(tid, 1);
			else
				page_pointer->release(tid, 0);

			delete page_pointer;
		}

		return 0;
	}

	vector<int>* NamedItems::getItems(TransactionID* tid)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "getItems(transactionID=%i, transactionTimeStamp=%i)\n", tid->getId(), tid->getTimeStamp());
#endif

		return getItems(tid, "");
	}

	template<typename Operation, typename DataType>
	vector<int>* NamedItems::getItemsByAnything(TransactionID* tid, Operation findTest, DataType thingToFind, vector<Indexes::RootEntry*> *indexContent, vector<const ix_entry *> *entries) {
		vector<int>* roots = new vector<int>();
		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;

		i = 1;

#ifdef IX_DEBUG
		debug_printf(*ec, "NamedItems::getItemsByAnything starts\n");
#endif
		while (i > 0)
		{

			if (buffer == NULL)
			    debug_printf(*ec, "NULL buffer!!\n");
			page_pointer = buffer->getPagePointer(tid, STORE_FILE_, (unsigned int) i);
			if (page_pointer == NULL)
			    debug_printf(*ec, "NULL page pointer (buffer not started?)\n");
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IX_NULLVALUE)
				i = 0;
			else if (page->entries == 0)
				i++;
			else
			{

				offset = sizeof(ixr_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					ix_entry* entry = (ix_entry*) (page_buf + offset);
					//ec->printf("@@@@@@@ !!! @@@@@@@ add_t: %i, l_id: %i del_t: %i, cur_tran: %i, name: %s\n", entry->add_t, entry->l_id, entry->del_t, entry->cur_tran, entry->name);
					if (indexContent != NULL) {
						//indeksy potrzebuja dostac wszystkie rooty, niezaleznie od tego czy sa widoczne dla danej transakcji
						indexContent->push_back(new Indexes::RootEntry(entry->add_t, entry->del_t, entry->cur_tran, entry->l_id));
					}
					if
					(
						//(strlen(name) == 0 || strcmp(name, entry->name) == 0)
						findTest(thingToFind, entry)
						&& entry->add_t <= (int)tid->getTimeStamp()
						&& entry->del_t == STORE_IX_NULLVALUE
						&& (entry->cur_tran == STORE_IX_NULLVALUE || entry->cur_tran == tid->getId())
					)
					{
						if (entries != NULL)
						{    
						    const ix_entry *en = *&entry; 
						    debug_printf(*ec, "CopyingEntry: %s to %s\n", entry->name, en->name);
						    entries->push_back(en);						
						}
						roots->push_back(entry->l_id);
					}
					offset += entry->size;
				}

				i++;
			}

			page_pointer->release(tid, 0);
			delete page_pointer;
		}
		return roots;
	}
	

	vector<int>* Classes::getClassByInvariant(TransactionID* tid, const char* invariantName) {
#ifdef IX_DEBUG
		debug_printf(*ec, "getClassByInvariant(invariantName=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", invariantName, tid->getId(), tid->getTimeStamp());
#endif
		//return getItemsByAnything(findByName, invariantName, transactionID, transactionTimeStamp);
		return getItemsByAnything(tid, findByInvariantName, invariantName);
	}
	
	vector<int>* Interfaces::getInterfaceByObjectName(TransactionID* tid, const char* objectName, vector<const ix_entry*>* entries)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "getInterfaceByObjectName(objectName=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", objectName, tid->getId(), tid->getTimeStamp());
#endif
		return getItemsByAnything(tid, findByObjectName, objectName, NULL, entries);	
	}

	vector<int>* NamedItems::getByNameWithEntries(TransactionID* tid, const char* name, vector<const ix_entry*>* entries)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "getInterfaceName(name=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", name, tid->getId(), tid->getTimeStamp());
#endif
		return getItemsByAnything(tid, findByName, name, NULL, entries);	
	}



	vector<int>* NamedRoots::getRootsWithBegin(TransactionID* tid, const char* nameBegin) {
#ifdef IX_DEBUG
		debug_printf(*ec, "getRootsWithBegin(nameBegin=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", nameBegin, tid->getId(), tid->getTimeStamp());
#endif
		return getItemsByAnything(tid, findNamesWithBegin, nameBegin);
	}

	vector<int>* NamedItems::getItems(TransactionID* tid, const char* name, vector<Indexes::RootEntry*> *indexContent)
	{
#ifdef IX_DEBUG
		debug_printf(*ec, "getItems(name=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", name, tid->getId(), tid->getTimeStamp());
#endif

		return getItemsByAnything(tid, findByName, name, indexContent);

		/* old version */
		/*vector<int>* roots = new vector<int>();
		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;
		ix_entry* entry = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(STORE_FILE_, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IX_NULLVALUE)
				i = 0;
			else if (page->entries == 0)
				i++;
			else
			{
				offset = sizeof(ixr_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (ix_entry*) (page_buf + offset);
					//ec->printf("@@@@@@@ !!! @@@@@@@ add_t: %i, l_id: %i del_t: %i, cur_tran: %i, name: %s\n", entry->add_t, entry->l_id, entry->del_t, entry->cur_tran, entry->name);
					if
					(
						(strlen(name) == 0 || strcmp(name, entry->name) == 0)
						&& entry->add_t <= transactionTimeStamp
						&& entry->del_t == STORE_IX_NULLVALUE
						&& (entry->cur_tran == STORE_IX_NULLVALUE || entry->cur_tran == transactionID)
					)
						roots->push_back(entry->l_id);

					offset += entry->size;
				}

				i++;
			}

			page_pointer->release(0);
			delete page_pointer;
		}

		return roots;*/
	}
}

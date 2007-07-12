#include "NamedItems.h"

namespace Store
{
	NamedItems::NamedItems()
	{
#ifdef IX_DEBUG
		this->ec = new ErrorConsole("Store: NamedItems");
#endif
	};

	NamedItems::~NamedItems()
	{
#ifdef IX_DEBUG
		delete(this->ec);
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
		ec->printf("initializeFile()\n");
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
		ec->printf("initializePage(pageID=%i)\n", pageID);
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

	int NamedItems::addItem(int logicalID, const char* name, int transactionID, int transactionTimeStamp)
	{
#ifdef IX_DEBUG
		ec->printf("addItem(logicalID=%i, name=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", logicalID, name, transactionID, transactionTimeStamp);
#endif

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
			page_pointer->aquire();

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
	};

	int NamedItems::removeItem(int logicalID, int transactionID, int transactionTimeStamp)
	{
#ifdef IX_DEBUG
		ec->printf("removeItem(logicalID=%i, transactionID=%i, transactionTimeStamp=%i)\n", logicalID, transactionID, transactionTimeStamp);
#endif

		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;
		ixr_entry* entry = 0;
		int entry_size = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(STORE_FILE_, (unsigned int) i);
			page_pointer->aquire();

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
					entry = (ixr_entry*) (page_buf + offset);

					if (entry->l_id == logicalID)
					{
						if (entry->cur_tran == STORE_IX_NULLVALUE)
						{
							entry->cur_tran = transactionID;
							entry->del_t = transactionTimeStamp;
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
				page_pointer->releaseSync(1);
			else
				page_pointer->release(0);

			delete page_pointer;
		}

		return 0;
	};

	int NamedItems::commitTransaction(int transactionID)
	{
#ifdef IX_DEBUG
		ec->printf("commitTransaction(transactionID=%i)\n", transactionID);
#endif

		return modifyTransaction(transactionID, 0);
	}

	int NamedItems::abortTransaction(int transactionID)
	{
#ifdef IX_DEBUG
		ec->printf("abortTransaction(transactionID=%i)\n", transactionID);
#endif

		return modifyTransaction(transactionID, 1);
	}

	int NamedItems::abortAllTransactions()
	{
#ifdef IX_DEBUG
		ec->printf("abortAllTransactions()\n");
#endif

		return modifyTransaction(0, 2);
	}

	int NamedItems::modifyTransaction(int transactionID, int mode)
	{
		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;
		ixr_entry* entry = 0;
		int entry_size = 0;
		int changed = 0;

		i = 1;
		while (i > 0)
		{
			changed = 0;
			page_pointer = buffer->getPagePointer(STORE_FILE_, (unsigned int) i);
			page_pointer->aquire();

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
					entry = (ixr_entry*) (page_buf + offset);
					entry_size = entry->size;

					if (entry->cur_tran == transactionID || (mode == 2 && entry->cur_tran != STORE_IX_NULLVALUE))
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
				page_pointer->releaseSync(1);
			else
				page_pointer->release(0);

			delete page_pointer;
		}

		return 0;
	}

	vector<int>* NamedItems::getItems(int transactionID, int transactionTimeStamp)
	{
#ifdef IX_DEBUG
		ec->printf("getItems(transactionID=%i, transactionTimeStamp=%i)\n", transactionID, transactionTimeStamp);
#endif

		return getItems("", transactionID, transactionTimeStamp);
	}

	vector<int>* NamedItems::getItems(const char* name, int transactionID, int transactionTimeStamp)
	{
#ifdef IX_DEBUG
		ec->printf("getItems(name=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", name, transactionID, transactionTimeStamp);
#endif

		vector<int>* roots = new vector<int>();
		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixr_page* page = 0;
		ixr_entry* entry = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(STORE_FILE_, (unsigned int) i);
			page_pointer->aquire();

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
					entry = (ixr_entry*) (page_buf + offset);

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

		return roots;
	}
}

#include "NamedRoots.h"

namespace Store
{
	NamedRoots::NamedRoots()
	{
#ifdef IXR_DEBUG
		this->ec = new ErrorConsole("Store: NamedRoots");
#endif
	};

	NamedRoots::~NamedRoots()
	{
#ifdef IXR_DEBUG
		delete(this->ec);
#endif
	};

	void NamedRoots::init(Buffer* buffer, LogManager* log)
	{
		this->buffer = buffer;
		this->log = log;
	}

	int NamedRoots::initializeFile(File* file)
	{
#ifdef IXR_DEBUG
		ec->printf("initializeFile()\n");
#endif

		char* buf = new char[STORE_PAGESIZE];
		ixr_header* header = (ixr_header*) buf;
		ixr_page* page = (ixr_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		header->page_hdr.page_id = 0;
		header->page_hdr.file_id = STORE_FILE_ROOTS;
		header->page_hdr.page_type = STORE_PAGE_ROOTSHEADER;
		header->page_hdr.timestamp = 0;
		header->last_page = 1;
		header->list_head = 1;
		file->writePage(STORE_FILE_ROOTS, 0, buf);

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_ROOTS;
		page->page_hdr.page_type = STORE_PAGE_ROOTSPAGE;
		page->free = STORE_PAGESIZE - sizeof(ixr_page);
		page->entries = 0;
		page->list_prev = STORE_IXR_NULLVALUE;
		page->list_next = STORE_IXR_NULLVALUE;

		for (unsigned int i = 1; i < STORE_IXR_INITIALPAGECOUNT; i++)
		{
			page->page_hdr.page_id = i;
			page->page_hdr.timestamp = i == 1 ? 0 : STORE_IXR_NULLVALUE;
			file->writePage(STORE_FILE_ROOTS, i, buf);
		}

		delete buf;

		return 0;
	};

	int NamedRoots::initializePage(unsigned int pageID, char* buf)
	{
#ifdef IXR_DEBUG
		ec->printf("initializePage(pageID=%i)\n", pageID);
#endif

		ixr_page* page = (ixr_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_ROOTS;
		page->page_hdr.page_type = STORE_PAGE_ROOTSPAGE;
		page->page_hdr.timestamp = STORE_IXR_NULLVALUE;
		page->page_hdr.page_id = pageID;
		page->free = STORE_PAGESIZE - sizeof(ixr_page);
		page->list_prev = STORE_IXR_NULLVALUE;
		page->list_next = STORE_IXR_NULLVALUE;

		return 0;
	}

	int NamedRoots::addRoot(int logicalID, const char* name, int transactionID, int transactionTimeStamp)
	{
#ifdef IXR_DEBUG
		ec->printf("addRoot(logicalID=%i, name=\"%s\", transactionID=%i)\n", logicalID, name, transactionID);
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
		if (name_len > STORE_IXR_NAMEMAXLEN)
			return 1;

		size_needed = sizeof(ixr_entry) + name_len + 1;
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);

		entry = (ixr_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = transactionID;
		entry->add_t = transactionTimeStamp;
		entry->del_t = STORE_IXR_NULLVALUE;
		memcpy(entry->name, name, name_len);

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(STORE_FILE_ROOTS, (unsigned int) i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->free >= size_needed)
			{
				memcpy(page_buf + (STORE_PAGESIZE - page->free), entry_buf, size_needed);
				page->free -= size_needed;
				page->entries++;
				page->page_hdr.timestamp = 123456789;

				i = 0;
			}
			else
				i++;

			page_pointer->release();
			delete page_pointer;
		}

		delete entry_buf;

		return 0;
	};

	int NamedRoots::removeRoot(int logicalID, int transactionID, int transactionTimeStamp)
	{
#ifdef IXR_DEBUG
		ec->printf("removeRoot(logicalID=%i, transactionID=%i)\n", logicalID, transactionID);
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
			page_pointer = buffer->getPagePointer(STORE_FILE_ROOTS, (unsigned int) i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IXR_NULLVALUE)
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
						if (entry->cur_tran == STORE_IXR_NULLVALUE)
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

						page->page_hdr.timestamp = 123456789;
						offset = STORE_PAGESIZE + 1;
						i = 0;
					}
					else
						offset += entry->size;
				}

				if (i > 0)
						i++;
			}

			page_pointer->release();
			delete page_pointer;
		}

		return 0;
	};

	int NamedRoots::commitTransaction(int transactionID)
	{
#ifdef IXR_DEBUG
		ec->printf("commitTransaction(transactionID=%i)\n", transactionID);
#endif

		return modifyTransaction(transactionID, 0);
	}

	int NamedRoots::abortTransaction(int transactionID)
	{
#ifdef IXR_DEBUG
		ec->printf("abortTransaction(transactionID=%i)\n", transactionID);
#endif

		return modifyTransaction(transactionID, 1);
	}

	int NamedRoots::abortAllTransactions()
	{
#ifdef IXR_DEBUG
		ec->printf("abortAllTransactions()\n");
#endif

		return modifyTransaction(0, 2);
	}

	int NamedRoots::modifyTransaction(int transactionID, int mode)
	{
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
			page_pointer = buffer->getPagePointer(STORE_FILE_ROOTS, (unsigned int) i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IXR_NULLVALUE)
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

					if (entry->cur_tran == transactionID || (mode == 2 && entry->cur_tran != STORE_IXR_NULLVALUE))
					{
						// Commiting remove
						if (entry->del_t != STORE_IXR_NULLVALUE && mode == 0)
						{
							entry->del_t = 123456789;
							entry->cur_tran = STORE_IXR_NULLVALUE;
						}
						// Commiting add
						else if (entry->del_t == STORE_IXR_NULLVALUE && mode == 0)
						{
							entry->add_t = 123456789;
							entry->cur_tran = STORE_IXR_NULLVALUE;
						}
						// Aborting remove
						else if (entry->del_t != STORE_IXR_NULLVALUE && mode > 0)
						{
							entry->del_t = STORE_IXR_NULLVALUE;
							entry->cur_tran = STORE_IXR_NULLVALUE;
						}
						// Aborting add
						else if (entry->del_t == STORE_IXR_NULLVALUE && mode > 0)
						{
							memmove(page_buf + offset, page_buf + offset + entry_size, STORE_PAGESIZE - (offset + entry_size + page->free));
							page->free += entry_size;
							page->entries--;
							memset(page_buf + (STORE_PAGESIZE - page->free), 0, page->free);
							entry_size = 0;
						}

						page->page_hdr.timestamp = 123456789;
					}

					offset += entry_size;
				}

				i++;
			}

			page_pointer->release();
			delete page_pointer;
		}

		return 0;
	}

	vector<int>* NamedRoots::getRoots(int transactionID, int transactionTimeStamp)
	{
#ifdef IXR_DEBUG
		ec->printf("getRoots(transactionID=%i)\n", transactionID);
#endif

		return getRoots(0, transactionID, transactionTimeStamp);
	}

	vector<int>* NamedRoots::getRoots(const char* name, int transactionID, int transactionTimeStamp)
	{
#ifdef IXR_DEBUG
		ec->printf("getRoots(name=\"%s\", transactionID=%i)\n", name, transactionID);
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
			page_pointer = buffer->getPagePointer(STORE_FILE_ROOTS, (unsigned int) i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (ixr_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IXR_NULLVALUE)
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
						(!name || strcasecmp(name, entry->name) == 0)
						&& entry->add_t <= transactionTimeStamp
						&& entry->del_t == STORE_IXR_NULLVALUE
						&& (entry->cur_tran == STORE_IXR_NULLVALUE || entry->cur_tran == transactionID)
					)
						roots->push_back(entry->l_id);

					offset += entry->size;
				}

				i++;
			}

			page_pointer->release();
			delete page_pointer;
		}

		return roots;
	}
}

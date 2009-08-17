#include <Store/Views.h>

namespace Store
{
	Views::Views()
	{
#ifdef IXV_DEBUG
		this->ec = &ErrorConsole::get_instance(EC_STORE_VIEWS);
#endif
	};

	Views::~Views()
	{
#ifdef IXV_DEBUG
#endif
	};

	void Views::init(Buffer* buffer, LogManager* log)
	{
		this->buffer = buffer;
		this->log = log;
	}

	int Views::initializeFile(File* file)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "initializeFile()\n");
#endif

		char* buf = new char[STORE_PAGESIZE];
		ixv_header* header = (ixv_header*) buf;
		ixv_page* page = (ixv_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		header->page_hdr.page_id = 0;
		header->page_hdr.file_id = STORE_FILE_VIEWS;
		header->page_hdr.page_type = STORE_PAGE_VIEWSHEADER;
		header->page_hdr.timestamp = 0;
		file->writePage(STORE_FILE_VIEWS, 0, buf);

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_VIEWS;
		page->page_hdr.page_type = STORE_PAGE_VIEWSPAGE;
		page->free = STORE_PAGESIZE - sizeof(ixv_page);
		page->entries = 0;

		for (unsigned int i = 1; i < STORE_IXV_INITIALPAGECOUNT; i++)
		{
			page->page_hdr.page_id = i;
			page->page_hdr.timestamp = i == 1 ? 0 : STORE_IXV_NULLVALUE;
			file->writePage(STORE_FILE_VIEWS, i, buf);
		}

		delete buf;

		return 0;
	};

	int Views::initializePage(unsigned int pageID, char* buf)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "initializePage(pageID=%i)\n", pageID);
#endif

		ixv_page* page = (ixv_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_VIEWS;
		page->page_hdr.page_type = STORE_PAGE_VIEWSPAGE;
		page->page_hdr.timestamp = STORE_IXV_NULLVALUE;
		page->page_hdr.page_id = pageID;
		page->free = STORE_PAGESIZE - sizeof(ixv_page);

		return 0;
	}

	int Views::addView(TransactionID* tid, int logicalID, const char* name)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "addView(logicalID=%i, name=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n", logicalID, name, tid->getId(), tid->getTimeStamp());
#endif

		int i = 0;
		int name_len = 0;
		int size_needed = 0;
		char* entry_buf = 0;
		ixv_entry* entry = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixv_page* page = 0;

		name_len = strlen(name);
		if (name_len > STORE_IXV_NAMEMAXLEN)
			return 1;

		size_needed = sizeof(ixv_entry) + name_len + 1;
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);

		entry = (ixv_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = tid->getId();
		entry->add_t = tid->getTimeStamp();
		entry->del_t = STORE_IXV_NULLVALUE;
		memcpy(entry->name, name, name_len);

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(tid, STORE_FILE_VIEWS, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixv_page*) page_buf;

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

		delete entry_buf;

		return 0;
	};

	int Views::removeView(TransactionID* tid, int logicalID)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "removeView(logicalID=%i, tid->getId()=%i, tid->getTimeStamp()=%i)\n", logicalID, tid->getId(), tid->getTimeStamp());
#endif

		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixv_page* page = 0;
		ixv_entry* entry = 0;
		int entry_size = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(tid, STORE_FILE_VIEWS, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixv_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IXV_NULLVALUE)
				i = 0;
			else if (page->entries == 0)
				i++;
			else
			{
				offset = sizeof(ixv_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (ixv_entry*) (page_buf + offset);

					if (entry->l_id == logicalID)
					{
						if (entry->cur_tran == STORE_IXV_NULLVALUE)
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

	int Views::commitTransaction(TransactionID* tid)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "commitTransaction(tid->getId()=%i)\n", tid->getId());
#endif

		return modifyTransaction(tid, 0);
	}

	int Views::abortTransaction(TransactionID* tid)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "abortTransaction(tid->getId()=%i)\n", tid->getId());
#endif

		return modifyTransaction(tid, 1);
	}

	int Views::abortAllTransactions()
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "abortAllTransactions()\n");
#endif

		return modifyTransaction(0, 2);
	}

	int Views::modifyTransaction(TransactionID* tid, int mode)
	{
		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixv_page* page = 0;
		ixv_entry* entry = 0;
		int entry_size = 0;
		int changed = 0;

		i = 1;
		while (i > 0)
		{
			changed = 0;
			page_pointer = buffer->getPagePointer(tid, STORE_FILE_VIEWS, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixv_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IXV_NULLVALUE)
				i = 0;
			else if (page->entries == 0)
				i++;
			else
			{
				offset = sizeof(ixv_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (ixv_entry*) (page_buf + offset);
					entry_size = entry->size;

					if (entry->cur_tran == tid->getId() || (mode == 2 && entry->cur_tran != STORE_IXV_NULLVALUE))
					{
						// Commiting remove
						if (entry->del_t != STORE_IXV_NULLVALUE && mode == 0)
						{
							entry->del_t = log->getLogicalTimerValue();
							entry->cur_tran = STORE_IXV_NULLVALUE;
						}
						// Commiting add
						else if (entry->del_t == STORE_IXV_NULLVALUE && mode == 0)
						{
							entry->add_t = log->getLogicalTimerValue();
							entry->cur_tran = STORE_IXV_NULLVALUE;
						}
						// Aborting remove
						else if (entry->del_t != STORE_IXV_NULLVALUE && mode > 0)
						{
							entry->del_t = STORE_IXV_NULLVALUE;
							entry->cur_tran = STORE_IXV_NULLVALUE;
						}
						// Aborting add
						else if (entry->del_t == STORE_IXV_NULLVALUE && mode > 0)
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

	vector<int>* Views::getViews(TransactionID* tid)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "getViews(tid->getId()=%i, tid->getTimeStamp()=%i)\n", tid->getId(), tid->getTimeStamp());
#endif

		return getViews(tid, "");
	}

	vector<int>* Views::getViews(TransactionID* tid, const char* name)
	{
#ifdef IXV_DEBUG
		debug_printf(*ec, "getViews(name=\"%s\", tid->getId()=%i, tid->getTimeStamp()=%i)\n", name, tid->getId(), tid->getTimeStamp());
#endif

		vector<int>* views = new vector<int>();
		int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		ixv_page* page = 0;
		ixv_entry* entry = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = buffer->getPagePointer(tid, STORE_FILE_VIEWS, (unsigned int) i);
			page_pointer->aquire(tid);

			page_buf = page_pointer->getPage();
			page = (ixv_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_IXV_NULLVALUE)
				i = 0;
			else if (page->entries == 0)
				i++;
			else
			{
				offset = sizeof(ixv_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (ixv_entry*) (page_buf + offset);

					if
					(
						(strlen(name) == 0 || strcmp(name, entry->name) == 0)
						&& static_cast<unsigned long>(entry->add_t) <= tid->getTimeStamp()
						&& entry->del_t == STORE_IXV_NULLVALUE
						&& (entry->cur_tran == STORE_IXV_NULLVALUE || entry->cur_tran == tid->getId())
					)
						views->push_back(entry->l_id);

					offset += entry->size;
				}

				i++;
			}

			page_pointer->release(tid, 0);
			delete page_pointer;
		}

		return views;
	}
}

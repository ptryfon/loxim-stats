#include "NamedRoots.h"
#include <string.h>

namespace Store
{
	NamedRoots::NamedRoots(DBStoreManager* store)
	{
		this->store = store;
		this->ec = new ErrorConsole("Store: Roots");
	};

	NamedRoots::~NamedRoots()
	{
	};

	int NamedRoots::initializeFile(File* file)
	{
		ec->printf("Initializing roots data file\n");
	
		char* buf = new char[STORE_PAGESIZE];
		namedroots_header* header = (namedroots_header*) buf;
		namedroots_page* page = (namedroots_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		header->page_hdr.file_id = STORE_FILE_ROOTS;
		header->page_hdr.page_id = 0;
		header->page_hdr.page_type = STORE_PAGE_ROOTSHEADER;
		header->page_hdr.timestamp = STORE_ROOTS_NULLVALUE;
		file->writePage(STORE_FILE_ROOTS, 0, buf);

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_ROOTS;
		page->page_hdr.page_type = STORE_PAGE_ROOTSPAGE;
		page->page_hdr.timestamp = STORE_ROOTS_NULLVALUE;

		for (int i = 1; i < STORE_ROOTS_INITIALPAGECOUNT; i++)
		{
			page->page_hdr.page_id = i;
			page->free = STORE_PAGESIZE - sizeof(namedroots_page);
			file->writePage(STORE_FILE_ROOTS, i, buf);
		}

		delete buf;

		return 0;
	};

	int NamedRoots::initializePage(unsigned int pageID, char* buf)
	{
		ec->printf("Initializing roots data page: %i\n", pageID);

		namedroots_page* page = (namedroots_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_ROOTS;
		page->page_hdr.page_type = STORE_PAGE_ROOTSPAGE;
		page->page_hdr.timestamp = STORE_ROOTS_NULLVALUE;
		page->page_hdr.page_id = pageID;
		page->free = STORE_PAGESIZE - sizeof(namedroots_page);

		return 0;
	}

	int NamedRoots::addRoot(int logicalID, const char* name, int transactionID, int transactionTimeStamp)
	{
		ec->printf("Adding root %i:\"%s\" in transaction %i\n", logicalID, name, transactionID);

		unsigned int i = 0;
		int name_len = 0;
		int size_needed = 0;
		char* entry_buf = 0;
		namedroots_entry* entry = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		namedroots_page* page = 0;

		name_len = strlen(name);
		if (name_len > STORE_ROOTS_NAMEMAXLEN)
			return 1;

		size_needed = sizeof(namedroots_entry) + name_len + 1;
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);

		entry = (namedroots_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = transactionID;
		entry->add_t = transactionTimeStamp;
		entry->del_t = STORE_ROOTS_NULLVALUE;
		memcpy(entry->name, name, name_len);

		i = 1;
		while (i > 0)
		{
			page_pointer = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (namedroots_page*) page_buf;

			if (page->free >= size_needed)
			{
				memcpy(page_buf + (STORE_PAGESIZE - page->free), entry_buf, size_needed);
				page->free -= size_needed;

				// TODO: Get TimeStamp from logs
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
		ec->printf("Removing root %i in transaction %i\n", logicalID, transactionID);

		unsigned int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		namedroots_page* page = 0;
		namedroots_entry* entry = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (namedroots_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_ROOTS_NULLVALUE)
				i = 0;
			else
			{
				offset = sizeof(namedroots_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (namedroots_entry*) (page_buf + offset);

					if (entry->l_id == logicalID)
					{
						entry->cur_tran = transactionID;
						entry->del_t = transactionTimeStamp;

						// TODO: Get TimeStamp from logs
						page->page_hdr.timestamp = 123456789;

						offset = STORE_PAGESIZE;
						i = 0;
					}
					else
						offset += entry->size;

					if (i > 0)
						i++;
				}
			}

			page_pointer->release();
			delete page_pointer;
		}

		return 0;
	};

	int NamedRoots::commitTransaction(int transactionID)
	{
		ec->printf("Commiting transaction %i\n", transactionID);

		return modifyTransaction(transactionID, 0);
	}

	int NamedRoots::abortTransaction(int transactionID)
	{
		ec->printf("Aborting transaction %i\n", transactionID);

		return modifyTransaction(transactionID, 1);
	}

	int NamedRoots::abortAllTransactions()
	{
		return modifyTransaction(0, 2);
	}

	int NamedRoots::modifyTransaction(int transactionID, char mode)
	{
		unsigned int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		namedroots_page* page = 0;
		namedroots_entry* entry = 0;
		int entry_size = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (namedroots_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_ROOTS_NULLVALUE)
				i = 0;
			else
			{
				offset = sizeof(namedroots_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (namedroots_entry*) (page_buf + offset);

					if (mode == 2 || entry->cur_tran == transactionID)
					{
						entry->cur_tran = STORE_ROOTS_NULLVALUE;
						if (entry->del_t != STORE_ROOTS_NULLVALUE && mode > 0)
							entry->del_t = STORE_ROOTS_NULLVALUE;
						else if (entry->del_t == STORE_ROOTS_NULLVALUE && mode > 0)
						{
//							entry->del_t = entry->add_t;

							entry_size = entry->size;
							memmove(page_buf + offset, page_buf + offset + entry_size, STORE_PAGESIZE - (offset + entry_size + page->free));
							page->free += entry_size;
						}

						// TODO: Get TimeStamp from logs
						page->page_hdr.timestamp = 123456789;
					}

					offset += entry->size;
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
		return getRoots(0, transactionID, transactionTimeStamp);
	}

	vector<int>* NamedRoots::getRoots(const char* name, int transactionID, int transactionTimeStamp)
	{
		ec->printf("Retrieving roots in transaction %i\n", transactionID);

		vector<int>* roots = new vector<int>();
		unsigned int i = 0;
		int offset = 0;
		PagePointer* page_pointer = 0;
		char* page_buf = 0;
		namedroots_page* page = 0;
		namedroots_entry* entry = 0;

		i = 1;
		while (i > 0)
		{
			page_pointer = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, i);
			page_pointer->aquire();

			page_buf = page_pointer->getPage();
			page = (namedroots_page*) page_buf;

			if (page->page_hdr.timestamp == STORE_ROOTS_NULLVALUE)
				i = 0;
			else
			{
				offset = sizeof(namedroots_page);
				while (offset < STORE_PAGESIZE - page->free)
				{
					entry = (namedroots_entry*) (page_buf + offset);

					if
					(
						(!name || strcasecmp(name, entry->name) == 0)
						&& entry->del_t == STORE_ROOTS_NULLVALUE
						&& entry->add_t <= transactionTimeStamp
						&& (entry->cur_tran == STORE_ROOTS_NULLVALUE || entry->cur_tran == transactionID)
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

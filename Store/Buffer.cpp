/**
 * $Id: Buffer.cpp,v 1.8 2006-01-20 12:14:56 mk189406 Exp $
 *
 */
#include "Buffer.h"

using namespace std;

namespace Store
{
	Buffer::Buffer(DBStoreManager* store)
	{
		this->store = store;
		this->file = new File(store);
		this->started = 0;
	};

	Buffer::~Buffer()
	{
		if (started)
			stop();

		if (file)
		{
			delete file;
			file = 0;
		}
	};

	int Buffer::start()
	{
		if (started)
			return 0;

		file->start();

		started = 1;
		return 0;
	};

	int Buffer::stop()
	{
		if (!started)
			return 0;

		file->stop();

		started = 0;
		return 0;
	};

	PagePointer* Buffer::getPagePointer(unsigned short fileID, unsigned int pageID)
	{
		unsigned int pnum;
		buffer_page* n_page;

		if (!started)
			return 0;
		
		buffer_addr_t buffer_addr = make_pair(fileID, pageID);

		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		if (it != buffer_hash.end() && (*it).second.haspage)
			return new PagePointer(fileID, pageID, (*it).second.page, this);

		if ((pnum = file->hasPage(fileID, pageID)) >= pageID) {
			n_page = new buffer_page;
			n_page->page = new char[STORE_PAGESIZE];

			if (file->readPage(fileID, pageID, n_page->page) != 0) {
				delete n_page->page;
				return 0;
			}
			n_page->haspage = 1;
			n_page->dirty = 1;
			buffer_hash.insert(make_pair (make_pair (fileID, pageID), *n_page));
		} else {
			for (unsigned int i = pnum + 1; i <= pageID; i++) {
				n_page = new buffer_page;
				n_page->page = new char[STORE_PAGESIZE];
				switch (fileID) {
					case STORE_FILE_DEFAULT: 
						PageManager::initializePage(i, n_page->page); 
						break;

					case STORE_FILE_MAP: 
						store->getMap()->initializePage(i, n_page->page); 
						break;

					case STORE_FILE_ROOTS: 
						store->getRoots()->initializePage(i, n_page->page); 
						break;

					default:
						break;
				}
				
				n_page->haspage = 1;
				n_page->dirty = 1;
				buffer_hash.insert(make_pair (make_pair (fileID, pageID), *n_page));
			}
		}

		return new PagePointer(fileID, pageID, n_page->page, this);
	};

	int Buffer::writePage(unsigned short fileID, unsigned int pageID, char *pagePointer) {
		if (!started)
			return 0;

		return file->writePage(fileID, pageID, pagePointer);
	};
};

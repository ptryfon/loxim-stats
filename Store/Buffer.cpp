/**
 * $Id: Buffer.cpp,v 1.7 2005-12-16 09:47:55 mk189406 Exp $
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
		if (!started)
			return 0;
		
		buffer_addr_t buffer_addr = make_pair(fileID, pageID);

		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		if (it != buffer_hash.end() && (*it).second.haspage)
			return new PagePointer(fileID, pageID, (*it).second.page, this);

		buffer_page n_page;
		n_page.page = new char[STORE_PAGESIZE];

		if (file->readPage(fileID, pageID, n_page.page) != 0) {
			delete n_page.page;
			return 0;
		}

		n_page.haspage = 1;
		buffer_hash.insert(make_pair (make_pair (fileID, pageID), n_page));
		return new PagePointer(fileID, pageID, n_page.page, this);
	};

	int Buffer::writePage(unsigned short fileID, unsigned int pageID, char *pagePointer) {
		if (!started)
			return 0;

		return file->writePage(fileID, pageID, pagePointer);
	};
};

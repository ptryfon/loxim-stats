/**
 * $Id: Buffer.cpp,v 1.4 2005-11-24 22:21:53 mk189406 Exp $
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
			return STORE_ERR_SUCCESS;

		file->start();

		started = 1;
		return STORE_ERR_SUCCESS;
	};

	int Buffer::stop()
	{
		if (!started)
			return STORE_ERR_SUCCESS;

		file->stop();

		started = 0;
		return STORE_ERR_SUCCESS;
	};

	PagePointer* Buffer::getPagePointer(unsigned short fileID, unsigned int pageID)
	{
		if (!started)
			return 0;
		
		buffer_addr_t buffer_addr = make_pair(fileID, pageID);

		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		if (it != buffer_hash.end() && (*it).second.haspage)
      return new PagePointer(fileID, pageID, (*it).second.page);

		buffer_page n_page;
		n_page.page = new char[STORE_PAGESIZE];

		if (file->readPage(fileID, pageID, n_page.page) != STORE_ERR_SUCCESS) {
			delete n_page.page;
			return 0;
		}

		n_page.haspage = 1;
		buffer_hash.insert(make_pair (make_pair (fileID, pageID), n_page));
		return new PagePointer(fileID, pageID, n_page.page);
	};
};

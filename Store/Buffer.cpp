#include "Buffer.h"

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

		buffer_count = 64;
		buffer_table = (buffer_page*) new char[sizeof(buffer_page) * buffer_count];
		memset(buffer_table, 0, sizeof(buffer_page) * buffer_count);

		started = 1;
		return STORE_ERR_SUCCESS;
	};

	int Buffer::stop()
	{
		if (!started)
			return STORE_ERR_SUCCESS;

		delete buffer_table;

		file->stop();

		started = 0;
		return STORE_ERR_SUCCESS;
	};

	PagePointer* Buffer::getPagePointer(unsigned short fileID, unsigned int pageID)
	{
		if (!started)
			return 0;

		for (int i = 0; i < buffer_count; i++)
			if (buffer_table[i].haspage
				&& ((page_header* )buffer_table[i].page)->file_id == fileID
				&& ((page_header* )buffer_table[i].page)->page_id == pageID
			)
				return new PagePointer(fileID, pageID, buffer_table[i].page);

		for (int i = 0; i < buffer_count; i++)
			if (!buffer_table[i].haspage)
			{
				int err;

				if (!buffer_table[i].page)
					buffer_table[i].page = new char[STORE_PAGESIZE];

				if ((err = file->readPage(fileID, pageID, buffer_table[i].page)) != STORE_ERR_SUCCESS)
					return 0;

				buffer_table[i].haspage = 1;
				return new PagePointer(fileID, pageID, buffer_table[i].page);
			}

		return 0;
	};
};
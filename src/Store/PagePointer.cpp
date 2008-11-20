#include "PagePointer.h"

#include "Buffer.h"

namespace Store
{
	PagePointer::PagePointer(unsigned short fileID, unsigned int pageID, char* pagePointer, Buffer* buffer)
	{
		this->fileID = fileID;
		this->pageID = pageID;
		this->pagePointer = pagePointer;
		this->buffer = buffer;
	};

	unsigned short PagePointer::getFileID()
	{
		return fileID;
	};

	unsigned int PagePointer::getPageID()
	{
		return pageID;
	};

	int PagePointer::aquire(TransactionID* tid)
	{
		int err = buffer->acquirePage(tid, this);

		if (!err)
		{
			pageOriginal = new char[STORE_PAGESIZE];
			memcpy(pageOriginal, pagePointer, STORE_PAGESIZE);
		}
		else
			pageOriginal = 0;

		return err;
	};

	int PagePointer::release(TransactionID* tid, int dirty)
	{
		if (dirty && pageOriginal)
		{
			// log->write(***TRANSACTION ID***, fileID, pageID, pageOriginal, pagePointer);
		}

		if (pageOriginal)
		{
			delete [] pageOriginal;
			pageOriginal = 0;
		}

		return buffer->releasePage(tid, this);
	};

	int PagePointer::releaseSync(TransactionID* tid, int dirty)
	{
		if (dirty && pageOriginal)
		{
			// log->write(***TRANSACTION ID***, fileID, pageID, pageOriginal, pagePointer);
		}

		if (pageOriginal)
		{
			delete [] pageOriginal;
			pageOriginal = 0;
		}

		return buffer->releasePageSync(tid, this);
	};

	char* PagePointer::getPage()
	{
		return pagePointer;
	};

	void PagePointer::setPage(char *pagePointer) {
		this->pagePointer = pagePointer;
	};
};

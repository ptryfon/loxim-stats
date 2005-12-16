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

	int PagePointer::aquire()
	{
		return 0;
	};

	int PagePointer::release()
	{
		buffer->writePage(fileID, pageID, pagePointer);
		return 0;
	};

	char* PagePointer::getPage()
	{
		return pagePointer;
	};
};

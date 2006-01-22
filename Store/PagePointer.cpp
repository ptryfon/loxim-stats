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
		return buffer->aquirePage(fileID, pageID);
	};

	int PagePointer::release()
	{
		return buffer->releasePage(fileID, pageID);
	};

	char* PagePointer::getPage()
	{
		return pagePointer;
	};
};

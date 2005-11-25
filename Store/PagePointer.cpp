#include "PagePointer.h"

namespace Store
{
	PagePointer::PagePointer(unsigned short fileID, unsigned int pageID, char* pagePointer)
	{
		this->fileID = fileID;
		this->pageID = pageID;
		this->pagePointer = pagePointer;
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
		return 0;
	};

	char* PagePointer::getPage()
	{
		return 0;
	};
};

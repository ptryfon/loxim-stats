#include "PagePointer.h"

namespace Store
{
	PagePointer::PagePointer(unsigned short fileID, unsigned int pageID, char* pagePointer)
	{
		this->fileID = fileID;
		this->pageID = pageID;
		this->pagePointer = pagePointer;
	};

	int PagePointer::aquire()
	{
		return STORE_ERR_SUCCESS;
	};

	int PagePointer::release()
	{
		return STORE_ERR_SUCCESS;
	};

	char* PagePointer::getPage()
	{
		return 0;
	};
};

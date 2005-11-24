#include "PagePointer.h"
#include "Errors/Errors.h"

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

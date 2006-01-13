#include "PageManager.h"

namespace Store
{

	int PageManager::binarizeSize(ObjectPointer *obj)
	{
		return 0;
	}
	
	int PageManager::binarize(ObjectPointer *obj, unsigned char **buff)
	{
		unsigned char* binaryObject = new unsigned char[binarizeSize(obj)];
		int writePosition = 0;
		int asize = 0;
		unsigned char* adata = NULL;
		unsigned int tmp = 0;
		
		//LogicalID
		obj->getLogicalID()->toByteArray(&adata, &asize);
		for(int i=0; i<asize; i++)
			binaryObject[writePosition++] = adata[i];
		if(adata) delete[] adata;
		adata = NULL;
		
		//random
		tmp = (rand()%0x100) << 24 + (rand()%0x100) << 16 +
			(rand()%0x100) << 8 + (rand()%0x100);
		adata = reinterpret_cast<unsigned char*>(&tmp);
		for(unsigned int i=0; i<sizeof(unsigned int); i++)
			binaryObject[writePosition++] = adata[i];
		adata = NULL;
		
		string name = obj->getName();
		//namesize
		tmp = static_cast<unsigned int>(name.length());
		adata = reinterpret_cast<unsigned char*>(&tmp);
		for(unsigned int i=0; i<sizeof(unsigned int); i++)
			binaryObject[writePosition++] = adata[i];
		adata = NULL;

		//name
		asize = name.length();
		for(int i=0; i<asize; i++)
			binaryObject[writePosition++] =
				static_cast<unsigned char>(name[i]);
		
		//value
		obj->getValue()->toFullByteArray(&adata, &asize);
		for(int i=0; i<asize; i++)
			binaryObject[writePosition++] = adata[i];
		if(adata) delete[] adata;
		adata = NULL;

	
		return 0;
	}
	
	int PageManager::writeHeader(PagePointer *pPtr, page_header hdr)
	{
	
		return 0;
	}
	
	int PageManager::putObject(PagePointer *pPtr)
	{
	
		return 0;
	}
	
	int PageManager::unbinarize(unsigned char *binobj, ObjectPointer **newobj)
	{
	
		return 0;
	}
	
}

#include <protocol/ptools/Package.h>
#include <string.h>

using namespace protocol;

const int Package::serialize(char **data)
{
	PackageBufferWriter *writer=new PackageBufferWriter();
	serializeW(writer);
	int size=writer->getSize()+5;
	*data=(char*)malloc(size);
	if(!(*data))
	{
		delete writer;
		return -1;
	};
	memcpy(*data,writer->getPackageBuffer(getPackageType()),size);	
	delete writer;
	return size;
}

bool  Package::deserialize(char* data)
{
	if((uint8_t)data[0]!=getPackageType())
		return false; //Z³y rodzaj pakietu.	
			
	PackageBufferReader *reader=new PackageBufferReader(data);
	
	bool bRet=true;
	
	if(!deserializeR(reader))	
		bRet=false;
		
	if (reader->hasERROR())
		bRet=false;
		
	delete reader;
	return bRet;
}

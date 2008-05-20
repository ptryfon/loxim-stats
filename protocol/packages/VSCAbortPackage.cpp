#include <string.h>
#include "VSCAbortPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

VSCAbortPackage::VSCAbortPackage()
{
	reasonCode=0;
	reasonString=NULL;
}

VSCAbortPackage::VSCAbortPackage(uint32_t a_reasonCode, CharArray* a_reasonString)
{
	reasonCode=a_reasonCode;
	reasonString=a_reasonString;
}

VSCAbortPackage::VSCAbortPackage(uint32_t a_reasonCode, char* a_reasonString)
{
	reasonCode=a_reasonCode;
	reasonString=new CharArray(strdup(a_reasonString));
}
			
void VSCAbortPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint32(reasonCode);
	writer->writeSstring(reasonString);
}

bool  VSCAbortPackage::deserializeR(PackageBufferReader *reader)
{
	reasonCode   =reader->readUint32();
	reasonString =reader->readString();
	return true;
}

bool VSCAbortPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
 	VSCAbortPackage *pack=(VSCAbortPackage*)package;
 	
 	if (pack->getReasonCode()!=getReasonCode())	
 		return false;
 		
 	return CharArray::equal(getReasonString(), pack->getReasonString());
 }



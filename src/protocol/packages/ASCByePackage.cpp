#include <string.h>
#include "ASCByePackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

ASCByePackage::ASCByePackage(char* a_reason)
{
	reason=new CharArray(strdup(a_reason));
}

ASCByePackage::ASCByePackage(CharArray *a_reason)
{
	reason=a_reason;
}

ASCByePackage::~ASCByePackage()
{
	if (reason)
		delete reason;
}

			
void ASCByePackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeSstring(reason);
}

bool  ASCByePackage::deserializeR(PackageBufferReader *reader)
{
	reason=reader->readSstring();
	return true;
}

bool ASCByePackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
 	return CharArray::equal(((ASCByePackage*)package)->getReason(),getReason()); 		
 }


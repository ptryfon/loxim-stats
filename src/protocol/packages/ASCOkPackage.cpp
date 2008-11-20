#include <string.h>
#include "ASCOkPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;


ASCOkPackage::ASCOkPackage()
{
}
			
void ASCOkPackage::serializeW(PackageBufferWriter *writer)
{
}

bool  ASCOkPackage::deserializeR(PackageBufferReader *reader)
{
	return true;
}

bool ASCOkPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return true;
 }



#include <string.h>
#include "WSAuthorizedPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

WSAuthorizedPackage::WSAuthorizedPackage()
{
}
			
void WSAuthorizedPackage::serializeW(PackageBufferWriter *writer)
{
}

bool  WSAuthorizedPackage::deserializeR(PackageBufferReader *reader)
{
	return true;
}

bool WSAuthorizedPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return true;
 }

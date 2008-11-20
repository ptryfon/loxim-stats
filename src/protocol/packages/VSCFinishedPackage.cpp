#include <string.h>
#include "VSCFinishedPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

VSCFinishedPackage::VSCFinishedPackage()
{
}
			
void VSCFinishedPackage::serializeW(PackageBufferWriter *writer)
{
}

bool  VSCFinishedPackage::deserializeR(PackageBufferReader *reader)
{
	return true;
}

bool VSCFinishedPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return true;
 }



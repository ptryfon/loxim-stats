#include <string.h>
#include <protocol/packages/VSCFinishedPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

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



#include <string.h>
#include <protocol/packages/ASCOkPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

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



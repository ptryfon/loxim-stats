#include <string.h>
#include <protocol/packages/ASCPingPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;

ASCPingPackage::ASCPingPackage()
{
}
			
void ASCPingPackage::serializeW(PackageBufferWriter *writer)
{
}

bool  ASCPingPackage::deserializeR(PackageBufferReader *reader)
{
	return true;
}

bool ASCPingPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return true;
 }

#include <string.h>
#include <protocol/packages/ASCPongPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;

ASCPongPackage::ASCPongPackage()
{
}
			
void ASCPongPackage::serializeW(PackageBufferWriter *writer)
{
}

bool  ASCPongPackage::deserializeR(PackageBufferReader *reader)
{
	return true;
}

bool ASCPongPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return true;
 }
 

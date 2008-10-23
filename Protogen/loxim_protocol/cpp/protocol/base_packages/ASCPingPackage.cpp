#include <string.h>
#include "ASCPingPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

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

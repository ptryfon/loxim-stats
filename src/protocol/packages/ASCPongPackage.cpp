#include <string.h>
#include "ASCPongPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

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
 

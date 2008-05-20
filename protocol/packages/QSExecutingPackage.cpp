#include <string.h>
#include "QSExecutingPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

QSExecutingPackage::QSExecutingPackage()
{
}
			
void QSExecutingPackage::serializeW(PackageBufferWriter *writer)
{
}

bool  QSExecutingPackage::deserializeR(PackageBufferReader *reader)
{
	return true;
}

bool QSExecutingPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return true;
 }



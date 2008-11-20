#include <string.h>
#include <protocol/packages/QSExecutingPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

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



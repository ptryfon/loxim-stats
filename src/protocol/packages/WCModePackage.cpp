#include <string.h>
#include <protocol/packages/WCModePackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;

WCModePackage::WCModePackage(uint64_t a_mode)
{
	mode=a_mode;
}
			
void WCModePackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint64(mode);
}

bool  WCModePackage::deserializeR(PackageBufferReader *reader)
{
	mode=reader->readUint64();	
	return true;
}

bool WCModePackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return ((WCModePackage*)package)->getMode()==getMode();
 }

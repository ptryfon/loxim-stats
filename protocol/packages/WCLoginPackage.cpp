#include <string.h>
#include "WCLoginPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

WCLoginPackage::WCLoginPackage(uint64_t a_authmode)
{
	auth_mode=a_authmode;
}

WCLoginPackage::WCLoginPackage()
{
	auth_mode=0;
}
			
void WCLoginPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint64(auth_mode);
}

bool  WCLoginPackage::deserializeR(PackageBufferReader *reader)
{	
	auth_mode=reader->readUint64();	
	return true;
}


bool WCLoginPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
   	return ((WCLoginPackage*)package)->getAuth_mode()==getAuth_mode();
 }

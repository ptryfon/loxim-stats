#include <string.h>
#include "WCPasswordPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

WCPasswordPackage::WCPasswordPackage(CharArray* a_login, CharArray* a_password)
{
	login=a_login;
	password=a_password;
}

WCPasswordPackage::WCPasswordPackage()
{
	login=NULL;
	password=NULL;
}
		

WCPasswordPackage::WCPasswordPackage(char* a_login,char* a_password)
{
	login=new CharArray(strdup(a_login));
	password=new CharArray(strdup(a_password));	
}

WCPasswordPackage::~WCPasswordPackage()
{
	if (login) delete login;
	if (password) delete password;
}
			
void WCPasswordPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeSstring(login);
	writer->writeSstring(password);
}

bool  WCPasswordPackage::deserializeR(PackageBufferReader *reader)
{
	login=reader->readSstring();
	password=reader->readSstring();
	return true;
}

bool WCPasswordPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
 	if(!CharArray::equal(((WCPasswordPackage*)package)->getLogin(),getLogin()))
 		return false;
 		
 	if(!CharArray::equal(((WCPasswordPackage*)package)->getPassword(),getPassword()))
 		return false;
 		
 	return true;
 }



#include <string.h>
#include <protocol/packages/ASCSetOptPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;

ASCSetOptPackage::ASCSetOptPackage(CharArray* a_key,CharArray* a_value)
{
	key=a_key;
	value=a_value;
}

ASCSetOptPackage::ASCSetOptPackage(char* a_key,char* a_value)
{
	key=new CharArray(strdup(a_key));
	value=new CharArray(strdup(a_value));
}

ASCSetOptPackage::~ASCSetOptPackage()
{
	if (key) delete key;
	if (value) delete value;
}

			
void ASCSetOptPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeSstring(key);
	writer->writeString(value);
}

bool  ASCSetOptPackage::deserializeR(PackageBufferReader *reader)
{
	key=reader->readSstring();
	value=reader->readString();		
	return true;
}

bool ASCSetOptPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
 	if(!CharArray::equal(((ASCSetOptPackage*)package)->getKey(),getKey()))
 		return false;
 		
 	if(!CharArray::equal(((ASCSetOptPackage*)package)->getValue(),getValue()))
 		return false;
 		
 	return true;
 }

#include <string.h>
#include <stdlib.h>
#include "W_c_loginPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

W_c_loginPackage::W_c_loginPackage(\
		enum Auth_methodsEnum a_auth_method
		)
{
	auth_method=a_auth_method;
}

W_c_loginPackage::W_c_loginPackage()
{
}

W_c_loginPackage::~W_c_loginPackage()
{
}

void W_c_loginPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	uint64_t tmp_auth_method;
	Auth_methodsFactory::getValueByEnum(auth_method, tmp_auth_method);
	writer->writeUint64(tmp_auth_method);
}

bool W_c_loginPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	Auth_methodsFactory::getEnumByValue(reader->readUint64(),auth_method);
	return true;
}

bool W_c_loginPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	W_c_loginPackage* p=(W_c_loginPackage*)_package;
	if (!(getAuth_method()==p->getAuth_method())) return false;
	return true;
}

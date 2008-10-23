#include <string.h>
#include <stdlib.h>
#include "W_c_passwordPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

W_c_passwordPackage::W_c_passwordPackage(\
		CharArray* a_login,\
		CharArray* a_password
		)
{
	login=a_login;
	password=a_password;
}

W_c_passwordPackage::W_c_passwordPackage()
{
	login=NULL;
	password=NULL;
}

W_c_passwordPackage::~W_c_passwordPackage()
{
	if (login) delete login;
	if (password) delete password;
}

void W_c_passwordPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeSstring(login);
	writer->writeSstring(password);
}

bool W_c_passwordPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	login=reader->readSstring();
	password=reader->readSstring();
	return true;
}

bool W_c_passwordPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	W_c_passwordPackage* p=(W_c_passwordPackage*)_package;
	if (!(CharArray::equal(getLogin(),p->getLogin()))) return false;
	if (!(CharArray::equal(getPassword(),p->getPassword()))) return false;
	return true;
}

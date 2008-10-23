#include <string.h>
#include <stdlib.h>
#include "W_c_authorizedPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

W_c_authorizedPackage::W_c_authorizedPackage()
{
}

W_c_authorizedPackage::~W_c_authorizedPackage()
{
}

void W_c_authorizedPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
}

bool W_c_authorizedPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	return true;
}

bool W_c_authorizedPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	W_c_authorizedPackage* p=(W_c_authorizedPackage*)_package;
	return true;
}

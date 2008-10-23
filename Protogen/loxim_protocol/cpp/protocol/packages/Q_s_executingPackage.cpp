#include <string.h>
#include <stdlib.h>
#include "Q_s_executingPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Q_s_executingPackage::Q_s_executingPackage()
{
}

Q_s_executingPackage::~Q_s_executingPackage()
{
}

void Q_s_executingPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
}

bool Q_s_executingPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	return true;
}

bool Q_s_executingPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Q_s_executingPackage* p=(Q_s_executingPackage*)_package;
	return true;
}

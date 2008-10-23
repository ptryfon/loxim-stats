#include <string.h>
#include <stdlib.h>
#include "VoidPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

VoidPackage::VoidPackage()
{
}

VoidPackage::~VoidPackage()
{
}

void VoidPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
}

bool VoidPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	return true;
}

bool VoidPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	VoidPackage* p=(VoidPackage*)_package;
	return true;
}

#include <string.h>
#include <stdlib.h>
#include "BagPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

BagPackage::BagPackage()
{
}

BagPackage::~BagPackage()
{
}

void BagPackage::serializeW(PackageBufferWriter *writer)
{
	CollectionPackage::serializeW(writer);
}

bool BagPackage::deserializeR(PackageBufferReader *reader)
{
	if (!CollectionPackage::deserializeR(reader)) return false;
	return true;
}

bool BagPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!CollectionPackage::equals(_package)) return false;
	BagPackage* p=(BagPackage*)_package;
	return true;
}

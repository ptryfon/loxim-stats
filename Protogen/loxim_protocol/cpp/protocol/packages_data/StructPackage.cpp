#include <string.h>
#include <stdlib.h>
#include "StructPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

StructPackage::StructPackage()
{
}

StructPackage::~StructPackage()
{
}

void StructPackage::serializeW(PackageBufferWriter *writer)
{
	CollectionPackage::serializeW(writer);
}

bool StructPackage::deserializeR(PackageBufferReader *reader)
{
	if (!CollectionPackage::deserializeR(reader)) return false;
	return true;
}

bool StructPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!CollectionPackage::equals(_package)) return false;
	StructPackage* p=(StructPackage*)_package;
	return true;
}

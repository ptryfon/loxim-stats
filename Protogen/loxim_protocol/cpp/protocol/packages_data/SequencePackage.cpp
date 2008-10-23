#include <string.h>
#include <stdlib.h>
#include "SequencePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

SequencePackage::SequencePackage()
{
}

SequencePackage::~SequencePackage()
{
}

void SequencePackage::serializeW(PackageBufferWriter *writer)
{
	CollectionPackage::serializeW(writer);
}

bool SequencePackage::deserializeR(PackageBufferReader *reader)
{
	if (!CollectionPackage::deserializeR(reader)) return false;
	return true;
}

bool SequencePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!CollectionPackage::equals(_package)) return false;
	SequencePackage* p=(SequencePackage*)_package;
	return true;
}

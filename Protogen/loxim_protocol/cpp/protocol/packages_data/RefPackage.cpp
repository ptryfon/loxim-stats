#include <string.h>
#include <stdlib.h>
#include "RefPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

RefPackage::RefPackage(\
		uint64_t a_valueId
		)
{
	valueId=a_valueId;
}

RefPackage::RefPackage()
{
	valueId=0;
}

RefPackage::~RefPackage()
{
}

void RefPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint64(valueId);
}

bool RefPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	valueId=reader->readUint64();
	return true;
}

bool RefPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	RefPackage* p=(RefPackage*)_package;
	if (!(getValueId()==p->getValueId())) return false;
	return true;
}

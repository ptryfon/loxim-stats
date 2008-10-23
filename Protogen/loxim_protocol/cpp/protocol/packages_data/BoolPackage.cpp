#include <string.h>
#include <stdlib.h>
#include "BoolPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

BoolPackage::BoolPackage(\
		bool a_value
		)
{
	value=a_value;
}

BoolPackage::BoolPackage()
{
	value=0;
}

BoolPackage::~BoolPackage()
{
}

void BoolPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeBool(value);
}

bool BoolPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readBool();
	return true;
}

bool BoolPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	BoolPackage* p=(BoolPackage*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

#include <string.h>
#include <stdlib.h>
#include "BobPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

BobPackage::BobPackage(\
		CharArray* a_value
		)
{
	value=a_value;
}

BobPackage::BobPackage()
{
	value=NULL;
}

BobPackage::~BobPackage()
{
	if (value) delete value;
}

void BobPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeString(value);
}

bool BobPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readString();
	return true;
}

bool BobPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	BobPackage* p=(BobPackage*)_package;
	if (!(CharArray::equal(getValue(),p->getValue()))) return false;
	return true;
}

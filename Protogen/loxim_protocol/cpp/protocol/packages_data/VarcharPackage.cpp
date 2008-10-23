#include <string.h>
#include <stdlib.h>
#include "VarcharPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

VarcharPackage::VarcharPackage(\
		CharArray* a_value
		)
{
	value=a_value;
}

VarcharPackage::VarcharPackage()
{
	value=NULL;
}

VarcharPackage::~VarcharPackage()
{
	if (value) delete value;
}

void VarcharPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeString(value);
}

bool VarcharPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readString();
	return true;
}

bool VarcharPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	VarcharPackage* p=(VarcharPackage*)_package;
	if (!(CharArray::equal(getValue(),p->getValue()))) return false;
	return true;
}

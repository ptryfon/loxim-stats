#include <string.h>
#include <stdlib.h>
#include "DoublePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

DoublePackage::DoublePackage(\
		double a_value
		)
{
	value=a_value;
}

DoublePackage::DoublePackage()
{
	value=0;
}

DoublePackage::~DoublePackage()
{
}

void DoublePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeDouble(value);
}

bool DoublePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readDouble();
	return true;
}

bool DoublePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	DoublePackage* p=(DoublePackage*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

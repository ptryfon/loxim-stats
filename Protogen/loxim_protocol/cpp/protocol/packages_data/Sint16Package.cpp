#include <string.h>
#include <stdlib.h>
#include "Sint16Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Sint16Package::Sint16Package(\
		int16_t a_value
		)
{
	value=a_value;
}

Sint16Package::Sint16Package()
{
	value=0;
}

Sint16Package::~Sint16Package()
{
}

void Sint16Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeInt16(value);
}

bool Sint16Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readInt16();
	return true;
}

bool Sint16Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Sint16Package* p=(Sint16Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

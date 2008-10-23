#include <string.h>
#include <stdlib.h>
#include "Sint8Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Sint8Package::Sint8Package(\
		int8_t a_value
		)
{
	value=a_value;
}

Sint8Package::Sint8Package()
{
	value=0;
}

Sint8Package::~Sint8Package()
{
}

void Sint8Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeInt8(value);
}

bool Sint8Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readInt8();
	return true;
}

bool Sint8Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Sint8Package* p=(Sint8Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

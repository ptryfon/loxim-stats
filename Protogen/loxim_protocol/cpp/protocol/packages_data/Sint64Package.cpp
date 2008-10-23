#include <string.h>
#include <stdlib.h>
#include "Sint64Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Sint64Package::Sint64Package(\
		int64_t a_value
		)
{
	value=a_value;
}

Sint64Package::Sint64Package()
{
	value=0;
}

Sint64Package::~Sint64Package()
{
}

void Sint64Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeInt64(value);
}

bool Sint64Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readInt64();
	return true;
}

bool Sint64Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Sint64Package* p=(Sint64Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

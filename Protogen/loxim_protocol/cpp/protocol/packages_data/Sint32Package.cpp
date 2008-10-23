#include <string.h>
#include <stdlib.h>
#include "Sint32Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Sint32Package::Sint32Package(\
		int32_t a_value
		)
{
	value=a_value;
}

Sint32Package::Sint32Package()
{
	value=0;
}

Sint32Package::~Sint32Package()
{
}

void Sint32Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeInt32(value);
}

bool Sint32Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readInt32();
	return true;
}

bool Sint32Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Sint32Package* p=(Sint32Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

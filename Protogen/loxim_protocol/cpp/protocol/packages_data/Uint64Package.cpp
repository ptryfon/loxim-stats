#include <string.h>
#include <stdlib.h>
#include "Uint64Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Uint64Package::Uint64Package(\
		uint64_t a_value
		)
{
	value=a_value;
}

Uint64Package::Uint64Package()
{
	value=0;
}

Uint64Package::~Uint64Package()
{
}

void Uint64Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint64(value);
}

bool Uint64Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readUint64();
	return true;
}

bool Uint64Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Uint64Package* p=(Uint64Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

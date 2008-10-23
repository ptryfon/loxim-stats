#include <string.h>
#include <stdlib.h>
#include "Uint32Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Uint32Package::Uint32Package(\
		uint32_t a_value
		)
{
	value=a_value;
}

Uint32Package::Uint32Package()
{
	value=0;
}

Uint32Package::~Uint32Package()
{
}

void Uint32Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint32(value);
}

bool Uint32Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readUint32();
	return true;
}

bool Uint32Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Uint32Package* p=(Uint32Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

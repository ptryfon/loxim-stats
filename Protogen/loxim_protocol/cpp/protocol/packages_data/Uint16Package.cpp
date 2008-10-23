#include <string.h>
#include <stdlib.h>
#include "Uint16Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Uint16Package::Uint16Package(\
		uint16_t a_value
		)
{
	value=a_value;
}

Uint16Package::Uint16Package()
{
	value=0;
}

Uint16Package::~Uint16Package()
{
}

void Uint16Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint16(value);
}

bool Uint16Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readUint16();
	return true;
}

bool Uint16Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Uint16Package* p=(Uint16Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

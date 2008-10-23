#include <string.h>
#include <stdlib.h>
#include "Uint8Package.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Uint8Package::Uint8Package(\
		uint8_t a_value
		)
{
	value=a_value;
}

Uint8Package::Uint8Package()
{
	value=0;
}

Uint8Package::~Uint8Package()
{
}

void Uint8Package::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint8(value);
}

bool Uint8Package::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	value=reader->readUint8();
	return true;
}

bool Uint8Package::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Uint8Package* p=(Uint8Package*)_package;
	if (!(getValue()==p->getValue())) return false;
	return true;
}

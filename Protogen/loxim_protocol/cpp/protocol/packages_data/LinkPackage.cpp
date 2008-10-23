#include <string.h>
#include <stdlib.h>
#include "LinkPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

LinkPackage::LinkPackage(\
		int64_t a_valueId
		)
{
	valueId=a_valueId;
}

LinkPackage::LinkPackage()
{
	valueId=0;
}

LinkPackage::~LinkPackage()
{
}

void LinkPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeVaruint(valueId);
}

bool LinkPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	{bool b; valueId=reader->readVaruint(b);}
	return true;
}

bool LinkPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	LinkPackage* p=(LinkPackage*)_package;
	if (!(getValueId()==p->getValueId())) return false;
	return true;
}

#include <string.h>
#include <stdlib.h>
#include "External_refPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

External_refPackage::External_refPackage(\
		uint64_t a_valueId,\
		uint64_t a_stamp
		)
{
	valueId=a_valueId;
	stamp=a_stamp;
}

External_refPackage::External_refPackage()
{
	valueId=0;
	stamp=0;
}

External_refPackage::~External_refPackage()
{
}

void External_refPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint64(valueId);
	writer->writeUint64(stamp);
}

bool External_refPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	valueId=reader->readUint64();
	stamp=reader->readUint64();
	return true;
}

bool External_refPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	External_refPackage* p=(External_refPackage*)_package;
	if (!(getValueId()==p->getValueId())) return false;
	if (!(getStamp()==p->getStamp())) return false;
	return true;
}

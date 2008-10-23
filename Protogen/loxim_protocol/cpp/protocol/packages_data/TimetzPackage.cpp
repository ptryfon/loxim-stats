#include <string.h>
#include <stdlib.h>
#include "TimetzPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

TimetzPackage::TimetzPackage(\
		TimePackage* a_time,\
		int8_t a_tz
		)
{
	time=a_time;
	tz=a_tz;
}

TimetzPackage::TimetzPackage()
{
	time=NULL;
	tz=0;
}

TimetzPackage::~TimetzPackage()
{
	if(time) delete time;
	time=NULL;
}

void TimetzPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	time->serializeContent(writer);
	writer->writeInt8(tz);
}

bool TimetzPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	time=new TimePackage();
	if (!time->deserializeContent(reader)) return false;
	tz=reader->readInt8();
	return true;
}

bool TimetzPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	TimetzPackage* p=(TimetzPackage*)_package;
	if (!((!getTime() && ! p->getTime())||(getTime() && p->getTime() && (getTime()->equals(p->getTime()))))) return false;
	if (!(getTz()==p->getTz())) return false;
	return true;
}

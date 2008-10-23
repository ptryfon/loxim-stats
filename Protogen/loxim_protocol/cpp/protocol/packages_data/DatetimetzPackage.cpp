#include <string.h>
#include <stdlib.h>
#include "DatetimetzPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

DatetimetzPackage::DatetimetzPackage(\
		DatePackage* a_date,\
		TimePackage* a_time,\
		int8_t a_tz
		)
{
	date=a_date;
	time=a_time;
	tz=a_tz;
}

DatetimetzPackage::DatetimetzPackage()
{
	date=NULL;
	time=NULL;
	tz=0;
}

DatetimetzPackage::~DatetimetzPackage()
{
	if(date) delete date;
	date=NULL;
	if(time) delete time;
	time=NULL;
}

void DatetimetzPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	date->serializeContent(writer);
	time->serializeContent(writer);
	writer->writeInt8(tz);
}

bool DatetimetzPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	date=new DatePackage();
	if (!date->deserializeContent(reader)) return false;
	time=new TimePackage();
	if (!time->deserializeContent(reader)) return false;
	tz=reader->readInt8();
	return true;
}

bool DatetimetzPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	DatetimetzPackage* p=(DatetimetzPackage*)_package;
	if (!((!getDate() && ! p->getDate())||(getDate() && p->getDate() && (getDate()->equals(p->getDate()))))) return false;
	if (!((!getTime() && ! p->getTime())||(getTime() && p->getTime() && (getTime()->equals(p->getTime()))))) return false;
	if (!(getTz()==p->getTz())) return false;
	return true;
}

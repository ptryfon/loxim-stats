#include <string.h>
#include <stdlib.h>
#include "DatetimePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

DatetimePackage::DatetimePackage(\
		DatePackage* a_date,\
		TimePackage* a_time
		)
{
	date=a_date;
	time=a_time;
}

DatetimePackage::DatetimePackage()
{
	date=NULL;
	time=NULL;
}

DatetimePackage::~DatetimePackage()
{
	if(date) delete date;
	date=NULL;
	if(time) delete time;
	time=NULL;
}

void DatetimePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	date->serializeContent(writer);
	time->serializeContent(writer);
}

bool DatetimePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	date=new DatePackage();
	if (!date->deserializeContent(reader)) return false;
	time=new TimePackage();
	if (!time->deserializeContent(reader)) return false;
	return true;
}

bool DatetimePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	DatetimePackage* p=(DatetimePackage*)_package;
	if (!((!getDate() && ! p->getDate())||(getDate() && p->getDate() && (getDate()->equals(p->getDate()))))) return false;
	if (!((!getTime() && ! p->getTime())||(getTime() && p->getTime() && (getTime()->equals(p->getTime()))))) return false;
	return true;
}

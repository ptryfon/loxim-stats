#include <string.h>
#include <stdlib.h>
#include "DatePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

DatePackage::DatePackage(\
		int16_t a_year,\
		uint8_t a_month,\
		uint8_t a_day
		)
{
	year=a_year;
	month=a_month;
	day=a_day;
}

DatePackage::DatePackage()
{
	year=0;
	month=0;
	day=0;
}

DatePackage::~DatePackage()
{
}

void DatePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeInt16(year);
	writer->writeUint8(month);
	writer->writeUint8(day);
}

bool DatePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	year=reader->readInt16();
	month=reader->readUint8();
	day=reader->readUint8();
	return true;
}

bool DatePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	DatePackage* p=(DatePackage*)_package;
	if (!(getYear()==p->getYear())) return false;
	if (!(getMonth()==p->getMonth())) return false;
	if (!(getDay()==p->getDay())) return false;
	return true;
}

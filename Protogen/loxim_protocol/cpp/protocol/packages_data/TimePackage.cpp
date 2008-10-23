#include <string.h>
#include <stdlib.h>
#include "TimePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

TimePackage::TimePackage(\
		uint8_t a_hour,\
		uint8_t a_minuts,\
		uint8_t a_sec,\
		int16_t a_milis
		)
{
	hour=a_hour;
	minuts=a_minuts;
	sec=a_sec;
	milis=a_milis;
}

TimePackage::TimePackage()
{
	hour=0;
	minuts=0;
	sec=0;
	milis=0;
}

TimePackage::~TimePackage()
{
}

void TimePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint8(hour);
	writer->writeUint8(minuts);
	writer->writeUint8(sec);
	writer->writeInt16(milis);
}

bool TimePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	hour=reader->readUint8();
	minuts=reader->readUint8();
	sec=reader->readUint8();
	milis=reader->readInt16();
	return true;
}

bool TimePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	TimePackage* p=(TimePackage*)_package;
	if (!(getHour()==p->getHour())) return false;
	if (!(getMinuts()==p->getMinuts())) return false;
	if (!(getSec()==p->getSec())) return false;
	if (!(getMilis()==p->getMilis())) return false;
	return true;
}

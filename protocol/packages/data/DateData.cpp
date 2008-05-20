#include "DateData.h"

using namespace protocol;

DateData::DateData()
{
	year=0;
	month=0;
	day=0;
}

DateData::DateData(int16_t a_year, uint8_t a_month, uint8_t a_day)
{
	year=a_year;
	month=a_month;
	day=a_day;
}

DateData::~DateData()
{
}

bool DateData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	DateData *date=(DateData*)package;
	return (getYear()==date->getYear())&&(getMonth()==date->getMonth())&&(getDay()==date->getDay());
};
		
void DateData::serializeW(PackageBufferWriter *writter)
{
	writter->writeInt16(year);
	writter->writeUint8(month);
	writter->writeUint8(day);
};
				
/* Ma zwróciæ informacjê, czy proces deserializacji siê powiód³ */
bool DateData::deserializeR(PackageBufferReader *reader)
{
	year =reader->readInt16();
	month=reader->readUint8();
	day  =reader->readUint8();
	return true;
};

#include "DateTimeData.h"

using namespace protocol;

DateTimeData::DateTimeData()
{
	time=0;
	date=0;
}

DateTimeData::DateTimeData(TimeData* a_time, DateData* a_date)
{
	time=a_time;
	date=a_date;
}

DateTimeData::~DateTimeData()
{
	if (time)
		delete time;
		
	if (date)
		delete date;
}

bool DateTimeData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;
		
	DateTimeData *d=(DateTimeData*)package;
	if (!DataPart::equal(d->getDate(),getDate()))
		return false;
	return DataPart::equal(d->getDate(),getDate());
}
		
void DateTimeData::serializeW(PackageBufferWriter *writter)
{
	date->serializeW(writter);
	time->serializeW(writter);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool DateTimeData::deserializeR(PackageBufferReader *reader)
{
	if(!date->deserializeR(reader))
		return false;
	return time->deserializeR(reader);
};

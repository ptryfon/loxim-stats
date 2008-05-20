#include "DateTimeTZData.h"

using namespace protocol;

DateTimeTZData::DateTimeTZData()
{
	time=0;
	date=0;
}

DateTimeTZData::DateTimeTZData(TimeTZData* a_time, DateData* a_date)
{
	time=a_time;
	date=a_date;
}

DateTimeTZData::~DateTimeTZData()
{
	if (time)
		delete time;
		
	if (date)
		delete date;
}

bool DateTimeTZData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;
		
	DateTimeTZData *d=(DateTimeTZData*)package;
	if (!DataPart::equal(d->getDate(),getDate()))
		return false;
	return DataPart::equal(d->getDate(),getDate());
}
		
void DateTimeTZData::serializeW(PackageBufferWriter *writter)
{
	date->serializeW(writter);
	time->serializeW(writter);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool DateTimeTZData::deserializeR(PackageBufferReader *reader)
{
	if(!date->deserializeR(reader))
		return false;
	return time->deserializeR(reader);
};

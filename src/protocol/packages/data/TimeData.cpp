#include <protocol/packages/data/TimeData.h>

using namespace protocol;

TimeData::TimeData()
{
		hour=0;
		minuts=0;
		secs=0;
		milis=0;
}

TimeData::TimeData(
			uint8_t a_hour,
			uint8_t a_minuts,
			uint8_t a_secs,
			uint16_t a_milis)
{
		hour=a_hour;
		minuts=a_minuts;
		secs=a_secs;
		milis=a_milis;
}

TimeData::~TimeData()
{
}

bool TimeData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;
	TimeData* data=(TimeData*)package;		
	return 
		(data->getHour()==getHour())&&
		(data->getMinuts()==getMinuts())&&
		(data->getSecs()==getSecs())&&
		(data->getMilis()==getMilis());
};
		
void TimeData::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint8(hour);
	writter->writeUint8(minuts);
	writter->writeUint8(secs);
	writter->writeUint16(milis);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool TimeData::deserializeR(PackageBufferReader *reader)
{
	hour=reader->readUint8();
	minuts=reader->readUint8();
	secs=reader->readUint8();
	milis=reader->readUint16();
	return true;
};

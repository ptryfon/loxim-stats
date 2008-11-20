#include <protocol/packages/data/TimeTZData.h>

using namespace protocol;

TimeTZData::TimeTZData()
{
		hour=0;
		minuts=0;
		secs=0;
		milis=0;
		tz=0;
}

TimeTZData::TimeTZData(
			uint8_t a_hour,
			uint8_t a_minuts,
			uint8_t a_secs,
			uint16_t a_milis,
			int8_t	a_tz)
{
		hour=a_hour;
		minuts=a_minuts;
		secs=a_secs;
		milis=a_milis;
		tz=a_tz;
}

TimeTZData::~TimeTZData()
{
}

bool TimeTZData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;
	TimeTZData* data=(TimeTZData*)package;		
	return 
		(data->getHour()==getHour())&&
		(data->getMinuts()==getMinuts())&&
		(data->getSecs()==getSecs())&&
		(data->getMilis()==getMilis())&&
		(data->getTz()==getTz());
};
		
void TimeTZData::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint8(hour);
	writter->writeUint8(minuts);
	writter->writeUint8(secs);
	writter->writeUint16(milis);
	writter->writeInt8(tz);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool TimeTZData::deserializeR(PackageBufferReader *reader)
{
	hour	=reader->readUint8();
	minuts	=reader->readUint8();
	secs	=reader->readUint8();
	milis	=reader->readUint16();
	tz		=reader->readInt8();
	return true;
};

#ifndef TIMEDATA_H_
#define TIMEDATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_TIME	0x000B

namespace protocol{


class TimeData: public DataPart
{
	private:
		uint8_t hour;
		uint8_t minuts;
		uint8_t secs;
		uint16_t milis;
	
	public:
		TimeData();
		TimeData(
			uint8_t a_hour,
			uint8_t a_minuts,
			uint8_t a_secs,
			uint16_t a_milis);
		virtual ~TimeData();
		
		uint8_t getHour(){return hour;};
		uint8_t getMinuts(){return minuts;};
		uint8_t getSecs(){return secs;};
		uint8_t getMilis(){return milis;};
		
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_TIME;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*TIMEDATA_H_*/

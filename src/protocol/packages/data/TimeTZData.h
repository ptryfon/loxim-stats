#ifndef TIMETZDATA_H_
#define TIMETZDATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_TIMETZ	0x000D
namespace protocol{


class TimeTZData: public DataPart
{
	private:
		uint8_t hour;
		uint8_t minuts;
		uint8_t secs;
		uint16_t milis;
		int8_t   tz;
	
	public:
		TimeTZData();
		TimeTZData(
			uint8_t a_hour,
			uint8_t a_minuts,
			uint8_t a_secs,
			uint16_t a_milis,
			int8_t	a_tz);
		virtual ~TimeTZData();
		
		uint8_t getHour(){return hour;};
		uint8_t getMinuts(){return minuts;};
		uint8_t getSecs(){return secs;};
		uint8_t getMilis(){return milis;};
		uint8_t getTz(){return tz;};
		
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_TIMETZ;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*TIMETZDATA_H_*/

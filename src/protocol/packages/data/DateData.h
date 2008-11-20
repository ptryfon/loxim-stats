#ifndef DATEDATA_H_
#define DATEDATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_DATE	0x000A
namespace protocol{


class DateData: public DataPart
{
	private:
		int16_t year;
		uint8_t month;
		uint8_t day;
	
	public:
		DateData();
		/* a_month: 
		 * 		1-styczeñ
		 * 		12-grudzieñ 
		 */
		DateData(int16_t a_year, uint8_t a_month, uint8_t a_day);
		virtual ~DateData();
		
		int16_t getYear(){return year;};
		uint8_t getMonth(){return month;};
		uint8_t getDay(){return day;};
		
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_DATE;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*DATEDATA_H_*/

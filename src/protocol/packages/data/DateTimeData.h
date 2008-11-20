#ifndef DATETIMEDATA_H_
#define DATETIMEDATA_H_

#include <protocol/packages/data/DataPart.h>
#include <protocol/packages/data/TimeData.h>
#include <protocol/packages/data/DateData.h>

#define DATAPART_TYPE_DATETIME	0x000C
namespace protocol{


class DateTimeData: public DataPart
{
	private:
		TimeData*   time;
		DateData*	date;
	
	public:
		DateTimeData();
		DateTimeData(TimeData* a_time, DateData* a_date);
		virtual ~DateTimeData();
		
		TimeData*   getTime(){return time;};
		DateData*	getDate(){return date;};
			
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_DATETIME;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*DATETIMEDATA_H_*/

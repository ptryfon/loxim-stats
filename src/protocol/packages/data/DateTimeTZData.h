#ifndef DATETIMETZDATA_H_
#define DATETIMETZDATA_H_

#include "DataPart.h"
#include "TimeTZData.h"
#include "DateData.h"

#define DATAPART_TYPE_DATETIMETZ	0x000E
namespace protocol{


class DateTimeTZData: public DataPart
{
	private:
		TimeTZData*   time;
		DateData*	date;
	
	public:
		DateTimeTZData();
		DateTimeTZData(TimeTZData* a_time, DateData* a_date);
		virtual ~DateTimeTZData();
		
		TimeTZData*   getTime(){return time;};
		DateData*	getDate(){return date;};
			
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_DATETIMETZ;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*DATETIMETZDATA_H_*/

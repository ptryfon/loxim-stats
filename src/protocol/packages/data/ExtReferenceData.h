#ifndef EXTREFERENCEDATA_H_
#define EXTREFERENCEDATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_EXTREFERENCE	0x0087
namespace protocol{


class ExtReferenceData: public DataPart
{
	private:
		uint64_t value;
		uint64_t stamp;
	
	public:
		ExtReferenceData();
		ExtReferenceData(uint64_t a_value,uint64_t a_stamp);
		virtual ~ExtReferenceData();
		
		uint64_t getValue(){return value;};
		uint64_t getStamp(){return stamp;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_EXTREFERENCE;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*EXTREFERENCEDATA_H_*/

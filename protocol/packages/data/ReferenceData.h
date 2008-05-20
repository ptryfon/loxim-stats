#ifndef REFERENCEDATA_H_
#define REFERENCEDATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_REFERENCE	0x0086
namespace protocol{


class ReferenceData: public DataPart
{
	private:
		uint64_t value;
	
	public:
		ReferenceData();
		ReferenceData(uint64_t a_value);
		virtual ~ReferenceData();
		
		uint64_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_REFERENCE;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*REFERENCEDATA_H_*/

#ifndef SINT32DATA_H_
#define SINT32DATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_SINT32	0x0006
namespace protocol{


class Sint32Data: public DataPart
{
	private:
		int32_t value;
	
	public:
		Sint32Data();
		Sint32Data(int32_t a_value);
		virtual ~Sint32Data();
		
		int32_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_SINT32;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*SINT32DATA_H_*/

#ifndef SINT8DATA_H_
#define SINT8DATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_SINT8	0x0002

namespace protocol{


class Sint8Data: public DataPart
{
	private:
		int8_t value;
	
	public:
		Sint8Data();
		Sint8Data(int8_t a_value);
		virtual ~Sint8Data();
		
		int8_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_SINT8;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*SINT8DATA_H_*/

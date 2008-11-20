#ifndef SINT16DATA_H_
#define SINT16DATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_SINT16	0x0004
namespace protocol{


class Sint16Data: public DataPart
{
	private:
		int16_t value;
	
	public:
		Sint16Data();
		Sint16Data(int16_t a_value);
		virtual ~Sint16Data();
		
		int16_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_SINT16;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*SINT16DATA_H_*/

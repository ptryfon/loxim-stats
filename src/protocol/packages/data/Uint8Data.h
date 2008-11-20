#ifndef UINT8DATA_H_
#define UINT8DATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_UINT8	0x0001
namespace protocol{


class Uint8Data: public DataPart
{
	private:
		uint8_t value;
	
	public:
		Uint8Data();
		Uint8Data(uint8_t a_value);
		virtual ~Uint8Data();
		
		uint8_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_UINT8;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*UINT8DATA_H_*/

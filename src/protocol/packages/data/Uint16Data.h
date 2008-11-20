#ifndef UINT16DATA_H_
#define UINT16DATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_UINT16	0x0003
namespace protocol{


class Uint16Data: public DataPart
{
	private:
		uint16_t value;
	
	public:
		Uint16Data();
		Uint16Data(uint16_t a_value);
		virtual ~Uint16Data();
		
		uint16_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_UINT16;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*UINT16DATA_H_*/

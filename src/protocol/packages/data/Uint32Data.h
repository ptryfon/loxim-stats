#ifndef UINT32DATA_H_
#define UINT32DATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_UINT32	0x0005
namespace protocol{


class Uint32Data: public DataPart
{
	private:
		uint32_t value;
	
	public:
		Uint32Data();
		Uint32Data(uint32_t a_value);
		virtual ~Uint32Data();
		
		uint32_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_UINT32;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*UINT32DATA_H_*/

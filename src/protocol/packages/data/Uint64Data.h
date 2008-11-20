#ifndef UINT64DATA_H_
#define UINT64DATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_UINT64	0x0007
namespace protocol{


class Uint64Data: public DataPart
{
	private:
		uint64_t value;
	
	public:
		Uint64Data();
		Uint64Data(uint64_t a_value);
		virtual ~Uint64Data();
		
		uint64_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_UINT64;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*UINT64DATA_H_*/

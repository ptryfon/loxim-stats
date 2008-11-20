#ifndef SINT64DATA_H_
#define SINT64DATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_SINT64	0x0008
namespace protocol{


class Sint64Data: public DataPart
{
	private:
		int64_t value;
	
	public:
		Sint64Data();
		Sint64Data(int64_t a_value);
		virtual ~Sint64Data();
		
		int64_t getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_SINT64;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};

}
#endif /*SINT64DATA_H_*/

#ifndef VARCHARDATA_H_
#define VARCHARDATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_VARCHAR	0x0010
namespace protocol{


class VarcharData: public DataPart
{
	private:
		CharArray* value;
	
	public:
		VarcharData();
		VarcharData(CharArray* a_value);
		VarcharData(char* a_value);
		virtual ~VarcharData();
		
		CharArray* getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_VARCHAR;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*VARCHARDATA_H_*/

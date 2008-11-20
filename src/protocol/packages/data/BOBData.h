#ifndef BOBDATA_H_
#define BOBDATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_BOB	0x000F

namespace protocol{

class BOBData: public DataPart
{
	private:
		CharArray* value;
	
	public:
		BOBData();
		BOBData(CharArray* a_value);
		virtual ~BOBData();
		
		CharArray* getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_BOB;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};

}
#endif /*BOBDATA_H_*/

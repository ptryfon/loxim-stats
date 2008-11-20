#ifndef BOOLDATA_H_
#define BOOLDATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_BOOL	0x0009
namespace protocol{

class BoolData: public DataPart
{
	private:
		bool value;
	
	public:
		BoolData();
		BoolData(bool a_value);
		virtual ~BoolData();
		
		bool getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_BOOL;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*BOOLDATA_H_*/

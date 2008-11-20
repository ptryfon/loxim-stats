#ifndef VOIDDATA_H_
#define VOIDDATA_H_

#include <protocol/packages/data/DataPart.h>

#define DATAPART_TYPE_VOID	0x0080
namespace protocol{


class VOIDData: public DataPart
{
	public:
		VOIDData();
		virtual ~VOIDData();
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_VOID;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*VOIDDATA_H_*/

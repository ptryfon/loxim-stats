#ifndef LINKDATA_H_
#define LINKDATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_LINK	0x0081
namespace protocol{


class LinkData: public DataPart
{
	private:
		uint64_t valueId;
	
	public:
		LinkData();
		LinkData(uint64_t a_value);
		virtual ~LinkData();
		
		uint64_t getValueId(){return valueId;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_LINK;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*LINKDATA_H_*/

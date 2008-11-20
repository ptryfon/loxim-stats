#ifndef STRUCTDATA_H_
#define STRUCTDATA_H_

#include <protocol/packages/data/CollectionData.h>

#define DATAPART_TYPE_STRUCT	0x0083
namespace protocol{


class StructData: public CollectionData
{
	public:
		StructData();
		StructData(uint64_t a_count,DataPart** a_dataParts);
		virtual ~StructData();
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_STRUCT;};
};

}
#endif /*STRUCTDATA_H_*/

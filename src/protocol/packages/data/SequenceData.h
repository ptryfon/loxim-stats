#ifndef SEQUENCEDATA_H_
#define SEQUENCEDATA_H_

#include <protocol/packages/data/CollectionData.h>

#define DATAPART_TYPE_SEQUENCE	0x0085
namespace protocol{


class SequenceData: public CollectionData
{
	public:
		SequenceData();
		SequenceData(uint64_t a_count,DataPart** a_dataParts);
		virtual ~SequenceData();
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_SEQUENCE;};
};
}
#endif /*SEQUENCEDATA_H_*/

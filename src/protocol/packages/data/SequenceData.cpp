#include <protocol/packages/data/SequenceData.h>

using namespace protocol;

SequenceData::SequenceData():CollectionData()
{
};

SequenceData::SequenceData(uint64_t a_count,DataPart** a_dataParts): CollectionData(a_count,a_dataParts)
{
	
};

SequenceData::~SequenceData()
{
}

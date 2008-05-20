#include "StructData.h"

using namespace protocol;

StructData::StructData():CollectionData()
{
};

StructData::StructData(uint64_t a_count,DataPart** a_dataParts): CollectionData(a_count,a_dataParts)
{
	
};

StructData::~StructData()
{
}

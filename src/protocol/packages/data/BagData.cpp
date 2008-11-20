#include "BagData.h"

using namespace protocol;

BagData::BagData():CollectionData()
{
};

BagData::BagData(uint64_t a_count,DataPart** a_dataParts): CollectionData(a_count,a_dataParts)
{
	
};

BagData::~BagData()
{
}

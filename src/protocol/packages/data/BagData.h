#ifndef BAGDATA_H_
#define BAGDATA_H_

#include <protocol/packages/data/CollectionData.h>

#define DATAPART_TYPE_BAG	0x0084

namespace protocol{

class BagData: public CollectionData
{
	public:
		BagData();
		BagData(uint64_t a_count,DataPart** a_dataParts);
		virtual ~BagData();
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_BAG;};
};

}
#endif /*BAGDATA_H_*/

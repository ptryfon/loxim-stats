#include "ExtReferenceData.h"

using namespace protocol;

ExtReferenceData::ExtReferenceData()
{
	value=0;
}

ExtReferenceData::ExtReferenceData(uint64_t a_value,uint64_t a_stamp)
{
	value=a_value;
	stamp=a_stamp;
}

ExtReferenceData::~ExtReferenceData()
{
}

bool ExtReferenceData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;
				
	if (getValue()!=((ExtReferenceData*)package)->getValue())
		return false;
		
	if (getStamp()!=((ExtReferenceData*)package)->getStamp())
		return false;
		
	return true;
};
		
void ExtReferenceData::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint64(value);
	writter->writeUint64(stamp);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool ExtReferenceData::deserializeR(PackageBufferReader *reader)
{
	value=reader->readUint64();
	stamp=reader->readUint64();
	return true;
};

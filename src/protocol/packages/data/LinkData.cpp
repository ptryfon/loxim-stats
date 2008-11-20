#include <protocol/packages/data/LinkData.h>

using namespace protocol;

LinkData::LinkData()
{
	valueId=0;
}

LinkData::LinkData(uint64_t a_valueId)
{
	valueId=a_valueId;
}

LinkData::~LinkData()
{
}

bool LinkData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValueId()==((LinkData*)package)->getValueId();
};
		
void LinkData::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint64(valueId);
};
				
/*Ma zwr�ci� informacj�, czy proces deserializacji si� powi�d�*/
bool LinkData::deserializeR(PackageBufferReader *reader)
{
	valueId=reader->readUint64();
	return true;
};

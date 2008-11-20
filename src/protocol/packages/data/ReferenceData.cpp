#include <protocol/packages/data/ReferenceData.h>

using namespace protocol;

ReferenceData::ReferenceData()
{
	value=0;
}

ReferenceData::ReferenceData(uint64_t a_value)
{
	value=a_value;
}

ReferenceData::~ReferenceData()
{
}

bool ReferenceData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((ReferenceData*)package)->getValue();
};
		
void ReferenceData::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint64(value);
};
				
/*Ma zwr�ci� informacj�, czy proces deserializacji si� powi�d�*/
bool ReferenceData::deserializeR(PackageBufferReader *reader)
{
	value=reader->readUint64();
	return true;
};

#include <protocol/packages/data/BOBData.h>

using namespace protocol;

BOBData::BOBData()
{
	value=NULL;
}

BOBData::BOBData(CharArray* a_value)
{
	value=a_value;
}

BOBData::~BOBData()
{
	if (value)
		delete value;
}

bool BOBData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return CharArray::equal(getValue(),((BOBData*)package)->getValue());
};
		
void BOBData::serializeW(PackageBufferWriter *writter)
{
	writter->writeString(value);
};
				
/*Ma zwr�ci� informacj�, czy proces deserializacji si� powi�d�*/
bool BOBData::deserializeR(PackageBufferReader *reader)
{
	value=reader->readString();
	return true;
};

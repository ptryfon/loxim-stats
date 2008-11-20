#include "BoolData.h"

using namespace protocol;

BoolData::BoolData()
{
	value=false;
}

BoolData::BoolData(bool a_value)
{
	value=a_value;
}

BoolData::~BoolData()
{
}

bool BoolData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((BoolData*)package)->getValue();
};
		
void BoolData::serializeW(PackageBufferWriter *writter)
{
	writter->writeBool(value);
};
				
/*Ma zwr�ci� informacj�, czy proces deserializacji si� powi�d�*/
bool BoolData::deserializeR(PackageBufferReader *reader)
{
	value=reader->readBool();
	return true;
};

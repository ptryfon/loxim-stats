#include "DoubleData.h"

using namespace protocol;

DoubleData::DoubleData()
{
	value=0.0;
}

DoubleData::DoubleData(double a_value)
{
	value=a_value;
}

DoubleData::~DoubleData()
{
}

bool DoubleData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((DoubleData*)package)->getValue();
};
		
void DoubleData::serializeW(PackageBufferWriter *writter)
{
	writter->writeDouble(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool DoubleData::deserializeR(PackageBufferReader *reader)
{
	value=reader->readDouble();
	return true;
};

#include "Sint8Data.h"

using namespace protocol;

Sint8Data::Sint8Data()
{
	value=0;
}

Sint8Data::Sint8Data(int8_t a_value)
{
	value=a_value;
}

Sint8Data::~Sint8Data()
{
}

bool Sint8Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Sint8Data*)package)->getValue();
};
		
void Sint8Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeInt8(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Sint8Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readInt8();
	return true;
};

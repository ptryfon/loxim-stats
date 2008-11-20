#include "Uint8Data.h"

using namespace protocol;

Uint8Data::Uint8Data()
{
	value=0;
}

Uint8Data::Uint8Data(uint8_t a_value)
{
	value=a_value;
}

Uint8Data::~Uint8Data()
{
}

bool Uint8Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Uint8Data*)package)->getValue();
};
		
void Uint8Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint8(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Uint8Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readUint8();
	return true;
};

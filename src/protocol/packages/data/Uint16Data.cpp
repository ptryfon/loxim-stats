#include "Uint16Data.h"

using namespace protocol;

Uint16Data::Uint16Data()
{
	value=0;
}

Uint16Data::Uint16Data(uint16_t a_value)
{
	value=a_value;
}

Uint16Data::~Uint16Data()
{
}

bool Uint16Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Uint16Data*)package)->getValue();
};
		
void Uint16Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint16(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Uint16Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readUint16();
	return true;
};

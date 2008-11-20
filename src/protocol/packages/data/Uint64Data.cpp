#include "Uint64Data.h"

using namespace protocol;

Uint64Data::Uint64Data()
{
	value=0;
}

Uint64Data::Uint64Data(uint64_t a_value)
{
	value=a_value;
}

Uint64Data::~Uint64Data()
{
}

bool Uint64Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Uint64Data*)package)->getValue();
};
		
void Uint64Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint64(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Uint64Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readUint64();
	return true;
};

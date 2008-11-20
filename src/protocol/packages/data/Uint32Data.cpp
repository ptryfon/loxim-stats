#include <protocol/packages/data/Uint32Data.h>

using namespace protocol;

Uint32Data::Uint32Data()
{
	value=0;
}

Uint32Data::Uint32Data(uint32_t a_value)
{
	value=a_value;
}

Uint32Data::~Uint32Data()
{
}

bool Uint32Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Uint32Data*)package)->getValue();
};
		
void Uint32Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeUint32(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Uint32Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readUint32();
	return true;
};

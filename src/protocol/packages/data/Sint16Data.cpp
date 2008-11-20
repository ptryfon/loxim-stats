#include <protocol/packages/data/Sint16Data.h>

using namespace protocol;

Sint16Data::Sint16Data()
{
	value=0;
}

Sint16Data::Sint16Data(int16_t a_value)
{
	value=a_value;
}

Sint16Data::~Sint16Data()
{
}

bool Sint16Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Sint16Data*)package)->getValue();
};
		
void Sint16Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeInt16(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Sint16Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readInt16();
	return true;
};

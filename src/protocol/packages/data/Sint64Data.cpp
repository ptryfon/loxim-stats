#include <protocol/packages/data/Sint64Data.h>

using namespace protocol;

Sint64Data::Sint64Data()
{
	value=0;
}

Sint64Data::Sint64Data(int64_t a_value)
{
	value=a_value;
}

Sint64Data::~Sint64Data()
{
}

bool Sint64Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Sint64Data*)package)->getValue();
};
		
void Sint64Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeInt64(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Sint64Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readInt64();
	return true;
};

#include "Sint32Data.h"

using namespace protocol;

Sint32Data::Sint32Data()
{
	value=0;
}

Sint32Data::Sint32Data(int32_t a_value)
{
	value=a_value;
}

Sint32Data::~Sint32Data()
{
}

bool Sint32Data::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return getValue()==((Sint32Data*)package)->getValue();
};
		
void Sint32Data::serializeW(PackageBufferWriter *writter)
{
	writter->writeInt32(value);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool Sint32Data::deserializeR(PackageBufferReader *reader)
{
	value=reader->readInt32();
	return true;
};

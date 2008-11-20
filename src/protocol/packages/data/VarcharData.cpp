#include <protocol/packages/data/VarcharData.h>

#include <string.h>

using namespace protocol;

VarcharData::VarcharData()
{
	value=0;
}

VarcharData::VarcharData(CharArray* a_value)
{
	value=a_value;
}

VarcharData::VarcharData(char* str)
{
	value=new CharArray(strdup(str));
}


VarcharData::~VarcharData()
{
}

bool VarcharData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return CharArray::equal(getValue(),((VarcharData*)package)->getValue());
};
		
void VarcharData::serializeW(PackageBufferWriter *writter)
{
	writter->writeString(value);
};
				
/*Ma zwr�ci� informacj�, czy proces deserializacji si� powi�d�*/
bool VarcharData::deserializeR(PackageBufferReader *reader)
{
	value=reader->readString();
	return true;
};

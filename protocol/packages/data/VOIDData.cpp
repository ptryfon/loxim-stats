#include "VOIDData.h"

using namespace protocol;

VOIDData::VOIDData()
{
}

VOIDData::~VOIDData()
{
}

bool VOIDData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	return true;
};
		
void VOIDData::serializeW(PackageBufferWriter *writter)
{
};
				
/*Ma zwr�ci� informacj�, czy proces deserializacji si� powi�d�*/
bool VOIDData::deserializeR(PackageBufferReader *reader)
{
	return true;
};

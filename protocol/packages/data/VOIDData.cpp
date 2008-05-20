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
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool VOIDData::deserializeR(PackageBufferReader *reader)
{
	return true;
};

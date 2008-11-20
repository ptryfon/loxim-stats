#include <protocol/packages/data/BindingData.h>
#include <protocol/packages/data/DataFactory.h>

using namespace protocol;

BindingData::BindingData()
{
	bindingName=NULL;
	valueId=0;
	embadedDataType=0;
	dataPart=NULL;
}

BindingData::BindingData(
	CharArray*  a_bindingName,
	uint64_t	a_embadedDataType,
	DataPart*	a_dataPart)
{
	bindingName=a_bindingName;
	valueId=0;
	embadedDataType=a_embadedDataType;
	dataPart=a_dataPart;
};
		
BindingData::BindingData(
	uint64_t	a_valueId,
	uint64_t	a_embadedDataType,
	DataPart*	a_dataPart)
{
	bindingName =NULL;
	valueId     =a_valueId;
	embadedDataType    =a_embadedDataType;
	dataPart    =a_dataPart;
};

BindingData::~BindingData()
{
	if (bindingName)
		delete bindingName;
		
	if (dataPart)
		delete dataPart;
}

bool BindingData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	
	BindingData *data=(BindingData*)package;	
	
	if (!CharArray::equal(getBindingName(),data->getBindingName()))
		return false;
		
	if (getValueId()!=data->getValueId())
		return false;
		
	if (getEmbadedDataType()!=data->getEmbadedDataType())
		return false;
		
	return DataPart::equal(getDataPart(),data->getDataPart());
};
		
void BindingData::serializeW(PackageBufferWriter *writter)
{
	writter->writeSstring(bindingName);
	if(bindingName==NULL)
	{
		writter->writeVaruint(valueId);
	}
	writter->writeVaruint(embadedDataType);
	dataPart->serializeW(writter);
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool BindingData::deserializeR(PackageBufferReader *reader)
{
	bindingName = reader->readSstring();
	if	(!bindingName)
	{
		bool null;
		valueId     = reader->readVaruint(null);
		if(null)
			return false;
	}
	bool null;
	embadedDataType    =reader->readVaruint(null);
	if (null)
		return false;
	dataPart = DataFactory::createDataPart(embadedDataType);
	return dataPart->deserializeR(reader);
};

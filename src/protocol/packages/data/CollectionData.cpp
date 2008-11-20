#include <protocol/packages/data/CollectionData.h>

using namespace protocol;

#include <protocol/packages/data/DataFactory.h>

CollectionData::CollectionData()
{
	count=0;
	globalType=0;
	bnull_globalType=false;
	dataParts=NULL;
}

CollectionData::CollectionData(uint64_t a_count,DataPart** a_dataParts)
{
	count=a_count;
	if (count==0)
	{
		globalType=0;
		bnull_globalType=false;
		dataParts=NULL;
	}else
	{
		dataParts=a_dataParts;		
		globalType=dataParts[0]->getDataType();
		bnull_globalType=false;
		for(uint64_t i=0; i<count; i++)
		{
			if (dataParts[i]->getDataType()!=globalType)
			{
				/* Znaleziono dwa elementy w tablicy o ró¿nym typie
				 * Wiêc typ globalny nie istnieje.
				 * */
				globalType=0;
				bnull_globalType=true;
				return;
			}
		};
	};	
}

CollectionData::~CollectionData()
{
	if (dataParts)
	{
		for(uint64_t i=0; i<count; i++)
			delete dataParts[i];
		delete dataParts;
	}
}

bool CollectionData::equals(DataPart* package)
{
	if (package->getDataType()!=getDataType())
		return false;		
	
	CollectionData *data=(CollectionData*)package;
	if (getCount()!=data->getCount())
		return false;
	
	if (getBnull_globalType()!=data->getBnull_globalType())
		return false;
	
	if (getGlobalType()!=data->getGlobalType())
		return false;
		
	for(uint64_t i=0; i<count; i++)
	{
		if (!DataPart::equal(dataParts[i],data->getDataParts()[i]))
			return false;
	};
	return true;
};
		
void CollectionData::serializeW(PackageBufferWriter *writter)
{
	writter->writeVaruint(count);
	
	if (bnull_globalType)
		writter->writeVaruintNull();
	else
		writter->writeVaruint(globalType);
		
	for(uint64_t i=0; i<count; i++)
	{
		if (bnull_globalType)
			writter->writeVaruint(dataParts[i]->getDataType());
		dataParts[i]->serializeW(writter);
	};
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool CollectionData::deserializeR(PackageBufferReader *reader)
{
	bool null;
	count=reader->readVaruint(null);
	if(null)
		return false;
	
	globalType=reader->readVaruint(bnull_globalType);
	dataParts=new DataPart*[count];
	for(uint64_t i=0; i<count; i++)
	{
		uint64_t local_type=(bnull_globalType)?reader->readVaruint(null):globalType;
		if(null)
			return false;
		dataParts[i]=DataFactory::createDataPart(local_type);
		
		if (!dataParts[i]->deserializeR(reader))
			return false;
	};
	
	return true;
};

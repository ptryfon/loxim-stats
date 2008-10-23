#include "CollectionPackage.h"

#include "DataPackagesFactory.h"

using namespace protocol;

CollectionPackage::CollectionPackage()
{
	count=0;
	globalType=0;
	bnull_globalType=false;
	dataParts=NULL;
}

CollectionPackage::CollectionPackage(uint64_t a_count,Package** a_dataParts)
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
		globalType=dataParts[0]->getPackageType();
		bnull_globalType=false;
		for(uint64_t i=0; i<count; i++)
		{
			if (dataParts[i]->getPackageType()!=globalType)
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

CollectionPackage::~CollectionPackage()
{
	if (dataParts)
	{
		for(uint64_t i=0; i<count; i++)
			delete dataParts[i];
		delete dataParts;
	}
}

bool CollectionPackage::equals(Package* package)
{
	if (package->getPackageType()!=getPackageType())
		return false;		
	
	CollectionPackage *data=(CollectionPackage*)package;
	if (getCount()!=data->getCount())
		return false;
	
	if (getBnull_globalType()!=data->getBnull_globalType())
		return false;
	
	if (getGlobalType()!=data->getGlobalType())
		return false;
		
	for(uint64_t i=0; i<count; i++)
	{
		if (!Package::equal(dataParts[i],data->getDataParts()[i]))
			return false;
	};
	return true;
};
		
void CollectionPackage::serializeW(PackageBufferWriter *writter)
{
	writter->writeVaruint(count);
	
	if (bnull_globalType)
		writter->writeVaruintNull();
	else
		writter->writeVaruint(globalType);
		
	for(uint64_t i=0; i<count; i++)
	{
		if (bnull_globalType)
			writter->writeVaruint(dataParts[i]->getPackageType());
		dataParts[i]->serializeContent(writter);
	};
};
				
/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
bool CollectionPackage::deserializeR(PackageBufferReader *reader)
{
	bool null;
	count=reader->readVaruint(null);
	if(null)
		return false;
	
	globalType=reader->readVaruint(bnull_globalType);
	dataParts=new Package*[count];
	for(uint64_t i=0; i<count; i++)
	{
		uint64_t local_type=(bnull_globalType)?reader->readVaruint(null):globalType;
		if(null)
			return false;
		dataParts[i]=DataPackagesFactory::createPackageNotInit(local_type);
		
		if (!dataParts[i]->deserializeContent(reader))
			return false;
	};
	
	return true;
};

#include <string.h>
#include <protocol/packages/VSCSendValuePackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <protocol/packages/data/DataFactory.h>

#include <stdlib.h>

using namespace protocol;

VSCSendValuePackage::VSCSendValuePackage()
{
	valueId=0;
	flags=0;
	data=NULL;
}

VSCSendValuePackage::VSCSendValuePackage(uint64_t a_valueId, uint8_t a_flags, DataPart* a_data)
{
	valueId=a_valueId;
	flags=a_flags;
	data=a_data;
};

VSCSendValuePackage::~VSCSendValuePackage()
{
	if (data)
		delete data;
}

			
void VSCSendValuePackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeVaruint(valueId);
	writer->writeUint8(flags);
	writer->writeVaruint(data->getDataType());
	data->serializeW(writer);
}

bool  VSCSendValuePackage::deserializeR(PackageBufferReader *reader)
{
	bool null;
	valueId=reader->readVaruint(null);
	if(null) return false;
	
	flags=reader->readUint8();
	
	uint16_t datatype=reader->readVaruint(null);
	if(null) return false;
	
	data=DataFactory::createDataPart(datatype);
	return data->deserializeR(reader);
}

bool VSCSendValuePackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
 	VSCSendValuePackage *pack=(VSCSendValuePackage*)package;
 	
 	if (pack->getValueId()!=getValueId())
 		return false;
 		
 	if (pack->getFlags()!=getFlags())
 		return false;
 	
   	return DataPart::equal(pack->getData(), getData());
 }



#include <string.h>
#include <stdlib.h>
#include "V_sc_sendvaluePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

V_sc_sendvaluePackage::V_sc_sendvaluePackage(\
		int64_t a_value_id,\
		struct Send_value_flagsMap a_flags,\
		int64_t a_value_type,\
		Package* a_data
		)
{
	value_id=a_value_id;
	flags=a_flags;
	value_type=a_value_type;
	data=a_data;
}

V_sc_sendvaluePackage::V_sc_sendvaluePackage()
{
	value_id=0;
	memset(&flags,0,sizeof(flags));
	value_type=0;
	data=NULL;
}

V_sc_sendvaluePackage::~V_sc_sendvaluePackage()
{
	if(data) delete data;
	data=NULL;
}

void V_sc_sendvaluePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeVaruint(value_id);
	uint8_t tmp_flags;
	Send_value_flagsFactory::getValueByEnumMap(flags, tmp_flags);
	writer->writeUint8(tmp_flags);
	writer->writeVaruint(value_type);
	data->serializeContent(writer);
}

bool V_sc_sendvaluePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	{bool b; value_id=reader->readVaruint(b);}
	Send_value_flagsFactory::getEnumMapByValue(reader->readUint8(),flags);
	{bool b; value_type=reader->readVaruint(b);}
	data=DataPackagesFactory::createPackageContent(value_type,reader);
	return true;
}

bool V_sc_sendvaluePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	V_sc_sendvaluePackage* p=(V_sc_sendvaluePackage*)_package;
	if (!(getValue_id()==p->getValue_id())) return false;
	if (!(Send_value_flagsFactory::mapEquals(getFlags(),p->getFlags()))) return false;
	if (!(getValue_type()==p->getValue_type())) return false;
	if (!((!getData() && ! p->getData())||(getData() && p->getData() && (getData()->equals(p->getData()))))) return false;
	return true;
}

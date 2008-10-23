#include <string.h>
#include <stdlib.h>
#include "W_s_helloPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

W_s_helloPackage::W_s_helloPackage(\
		uint8_t a_protocol_major,\
		uint8_t a_protocol_minor,\
		uint8_t a_system_major,\
		uint8_t a_system_minor,\
		uint32_t a_max_package_size,\
		struct FeaturesMap a_features,\
		struct Auth_methodsMap a_auth_methods,\
		CharArray* a_salt
		)
{
	protocol_major=a_protocol_major;
	protocol_minor=a_protocol_minor;
	system_major=a_system_major;
	system_minor=a_system_minor;
	max_package_size=a_max_package_size;
	features=a_features;
	auth_methods=a_auth_methods;
	salt=a_salt;
}

W_s_helloPackage::W_s_helloPackage()
{
	protocol_major=0;
	protocol_minor=0;
	system_major=0;
	system_minor=0;
	max_package_size=0;
	memset(&features,0,sizeof(features));
	memset(&auth_methods,0,sizeof(auth_methods));
	salt=NULL;
}

W_s_helloPackage::~W_s_helloPackage()
{
	if (salt) delete salt;
}

void W_s_helloPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint8(protocol_major);
	writer->writeUint8(protocol_minor);
	writer->writeUint8(system_major);
	writer->writeUint8(system_minor);
	writer->writeUint32(max_package_size);
	uint64_t tmp_features;
	FeaturesFactory::getValueByEnumMap(features, tmp_features);
	writer->writeUint64(tmp_features);
	uint64_t tmp_auth_methods;
	Auth_methodsFactory::getValueByEnumMap(auth_methods, tmp_auth_methods);
	writer->writeUint64(tmp_auth_methods);
	writer->writeString(salt);
}

bool W_s_helloPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	protocol_major=reader->readUint8();
	protocol_minor=reader->readUint8();
	system_major=reader->readUint8();
	system_minor=reader->readUint8();
	max_package_size=reader->readUint32();
	FeaturesFactory::getEnumMapByValue(reader->readUint64(),features);
	Auth_methodsFactory::getEnumMapByValue(reader->readUint64(),auth_methods);
	salt=reader->readString();
	return true;
}

bool W_s_helloPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	W_s_helloPackage* p=(W_s_helloPackage*)_package;
	if (!(getProtocol_major()==p->getProtocol_major())) return false;
	if (!(getProtocol_minor()==p->getProtocol_minor())) return false;
	if (!(getSystem_major()==p->getSystem_major())) return false;
	if (!(getSystem_minor()==p->getSystem_minor())) return false;
	if (!(getMax_package_size()==p->getMax_package_size())) return false;
	if (!(FeaturesFactory::mapEquals(getFeatures(),p->getFeatures()))) return false;
	if (!(Auth_methodsFactory::mapEquals(getAuth_methods(),p->getAuth_methods()))) return false;
	if (!(CharArray::equal(getSalt(),p->getSalt()))) return false;
	return true;
}

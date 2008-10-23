#include <string.h>
#include <stdlib.h>
#include "W_c_helloPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

W_c_helloPackage::W_c_helloPackage(\
		int64_t a_pid,\
		CharArray* a_client_name,\
		CharArray* a_client_version,\
		CharArray* a_hostname,\
		CharArray* a_language,\
		enum CollationsEnum a_collation,\
		int8_t a_timezone
		)
{
	pid=a_pid;
	client_name=a_client_name;
	client_version=a_client_version;
	hostname=a_hostname;
	language=a_language;
	collation=a_collation;
	timezone=a_timezone;
}

W_c_helloPackage::W_c_helloPackage()
{
	pid=0;
	client_name=NULL;
	client_version=NULL;
	hostname=NULL;
	language=NULL;
	timezone=0;
}

W_c_helloPackage::~W_c_helloPackage()
{
	if (client_name) delete client_name;
	if (client_version) delete client_version;
	if (hostname) delete hostname;
	if (language) delete language;
}

void W_c_helloPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeInt64(pid);
	writer->writeSstring(client_name);
	writer->writeSstring(client_version);
	writer->writeSstring(hostname);
	writer->writeSstring(language);
	uint64_t tmp_collation;
	CollationsFactory::getValueByEnum(collation, tmp_collation);
	writer->writeUint64(tmp_collation);
	writer->writeInt8(timezone);
}

bool W_c_helloPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	pid=reader->readInt64();
	client_name=reader->readSstring();
	client_version=reader->readSstring();
	hostname=reader->readSstring();
	language=reader->readSstring();
	CollationsFactory::getEnumByValue(reader->readUint64(),collation);
	timezone=reader->readInt8();
	return true;
}

bool W_c_helloPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	W_c_helloPackage* p=(W_c_helloPackage*)_package;
	if (!(getPid()==p->getPid())) return false;
	if (!(CharArray::equal(getClient_name(),p->getClient_name()))) return false;
	if (!(CharArray::equal(getClient_version(),p->getClient_version()))) return false;
	if (!(CharArray::equal(getHostname(),p->getHostname()))) return false;
	if (!(CharArray::equal(getLanguage(),p->getLanguage()))) return false;
	if (!(getCollation()==p->getCollation())) return false;
	if (!(getTimezone()==p->getTimezone())) return false;
	return true;
}

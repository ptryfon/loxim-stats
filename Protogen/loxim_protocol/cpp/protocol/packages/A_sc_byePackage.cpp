#include <string.h>
#include <stdlib.h>
#include "A_sc_byePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

A_sc_byePackage::A_sc_byePackage(\
		enum Bye_reasonsEnum a_reason_code,\
		CharArray* a_reason_description
		)
{
	reason_code=a_reason_code;
	reason_description=a_reason_description;
}

A_sc_byePackage::A_sc_byePackage()
{
	reason_description=NULL;
}

A_sc_byePackage::~A_sc_byePackage()
{
	if (reason_description) delete reason_description;
}

void A_sc_byePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	uint32_t tmp_reason_code;
	Bye_reasonsFactory::getValueByEnum(reason_code, tmp_reason_code);
	writer->writeUint32(tmp_reason_code);
	writer->writeString(reason_description);
}

bool A_sc_byePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	Bye_reasonsFactory::getEnumByValue(reader->readUint32(),reason_code);
	reason_description=reader->readString();
	return true;
}

bool A_sc_byePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	A_sc_byePackage* p=(A_sc_byePackage*)_package;
	if (!(getReason_code()==p->getReason_code())) return false;
	if (!(CharArray::equal(getReason_description(),p->getReason_description()))) return false;
	return true;
}

#include <string.h>
#include <stdlib.h>
#include "V_sc_abortPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

V_sc_abortPackage::V_sc_abortPackage(\
		enum Abort_reason_codesEnum a_reasonCode,\
		CharArray* a_reasonString
		)
{
	reasonCode=a_reasonCode;
	reasonString=a_reasonString;
}

V_sc_abortPackage::V_sc_abortPackage()
{
	reasonString=NULL;
}

V_sc_abortPackage::~V_sc_abortPackage()
{
	if (reasonString) delete reasonString;
}

void V_sc_abortPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	uint32_t tmp_reasonCode;
	Abort_reason_codesFactory::getValueByEnum(reasonCode, tmp_reasonCode);
	writer->writeUint32(tmp_reasonCode);
	writer->writeString(reasonString);
}

bool V_sc_abortPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	Abort_reason_codesFactory::getEnumByValue(reader->readUint32(),reasonCode);
	reasonString=reader->readString();
	return true;
}

bool V_sc_abortPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	V_sc_abortPackage* p=(V_sc_abortPackage*)_package;
	if (!(getReasonCode()==p->getReasonCode())) return false;
	if (!(CharArray::equal(getReasonString(),p->getReasonString()))) return false;
	return true;
}

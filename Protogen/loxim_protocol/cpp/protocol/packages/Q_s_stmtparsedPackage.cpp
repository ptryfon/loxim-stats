#include <string.h>
#include <stdlib.h>
#include "Q_s_stmtparsedPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Q_s_stmtparsedPackage::Q_s_stmtparsedPackage(\
		uint64_t a_statement_id,\
		uint32_t a_paramsCnt
		)
{
	statement_id=a_statement_id;
	paramsCnt=a_paramsCnt;
}

Q_s_stmtparsedPackage::Q_s_stmtparsedPackage()
{
	statement_id=0;
	paramsCnt=0;
}

Q_s_stmtparsedPackage::~Q_s_stmtparsedPackage()
{
}

void Q_s_stmtparsedPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint64(statement_id);
	writer->writeUint32(paramsCnt);
}

bool Q_s_stmtparsedPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	statement_id=reader->readUint64();
	paramsCnt=reader->readUint32();
	return true;
}

bool Q_s_stmtparsedPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Q_s_stmtparsedPackage* p=(Q_s_stmtparsedPackage*)_package;
	if (!(getStatement_id()==p->getStatement_id())) return false;
	if (!(getParamsCnt()==p->getParamsCnt())) return false;
	return true;
}

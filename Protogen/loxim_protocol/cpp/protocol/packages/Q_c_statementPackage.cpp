#include <string.h>
#include <stdlib.h>
#include "Q_c_statementPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Q_c_statementPackage::Q_c_statementPackage(\
		struct Statement_flagsMap a_flags,\
		CharArray* a_statement
		)
{
	flags=a_flags;
	statement=a_statement;
}

Q_c_statementPackage::Q_c_statementPackage()
{
	memset(&flags,0,sizeof(flags));
	statement=NULL;
}

Q_c_statementPackage::~Q_c_statementPackage()
{
	if (statement) delete statement;
}

void Q_c_statementPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	uint64_t tmp_flags;
	Statement_flagsFactory::getValueByEnumMap(flags, tmp_flags);
	writer->writeUint64(tmp_flags);
	writer->writeString(statement);
}

bool Q_c_statementPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	Statement_flagsFactory::getEnumMapByValue(reader->readUint64(),flags);
	statement=reader->readString();
	return true;
}

bool Q_c_statementPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Q_c_statementPackage* p=(Q_c_statementPackage*)_package;
	if (!(Statement_flagsFactory::mapEquals(getFlags(),p->getFlags()))) return false;
	if (!(CharArray::equal(getStatement(),p->getStatement()))) return false;
	return true;
}

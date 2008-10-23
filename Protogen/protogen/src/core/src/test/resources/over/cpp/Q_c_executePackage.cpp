#include <string.h>
#include <stdlib.h>
#include "Q_c_executePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Q_c_executePackage::Q_c_executePackage(\
		uint64_t a_statement_id,\
		struct Statement_flagsMap a_flags,\
		uint32_t a_paramsCnt,\
		int64_t *a_params
		)
{
	statement_id=a_statement_id;
	flags=a_flags;
	paramsCnt=a_paramsCnt;
	params=a_params;
}

Q_c_executePackage::Q_c_executePackage()
{
	statement_id=0;
	memset(&flags,0,sizeof(flags));
	paramsCnt=0;
	params=0;
}

Q_c_executePackage::~Q_c_executePackage()
{
}

void Q_c_executePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeUint64(statement_id);
	uint64_t tmp_flags;
	Statement_flagsFactory::getValueByEnumMap(flags, tmp_flags);
	writer->writeUint64(tmp_flags);
	writer->writeUint32(paramsCnt);
	for(int i=0; i<paramsCnt; i++)
	{
		writer->writeVaruint(params[i]);
	}
}

bool Q_c_executePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	statement_id=reader->readUint64();
	Statement_flagsFactory::getEnumMapByValue(reader->readUint64(),flags);
	paramsCnt=reader->readUint32();
	for(int i=0; i<paramsCnt; i++)
	{
		bool b; 
		params[i]=reader->readVaruint(b);
	}
	return true;
}

bool Q_c_executePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Q_c_executePackage* p=(Q_c_executePackage*)_package;
	if (!(getStatement_id()==p->getStatement_id())) return false;
	if (!(Statement_flagsFactory::mapEquals(getFlags(),p->getFlags()))) return false;
	if (!(getParamsCnt()==p->getParamsCnt())) return false;
	for(int i=0; i<paramsCnt; i++)
		if (!(getParams()[i]==p->getParams()[i]))
			return false;
	return true;
}

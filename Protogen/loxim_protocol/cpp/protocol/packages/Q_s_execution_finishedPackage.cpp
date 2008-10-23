#include <string.h>
#include <stdlib.h>
#include "Q_s_execution_finishedPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

Q_s_execution_finishedPackage::Q_s_execution_finishedPackage(\
		int64_t a_modAtomPointerCnt,\
		int64_t a_delCnt,\
		int64_t a_newRootsCnt,\
		int64_t a_insertsCnt
		)
{
	modAtomPointerCnt=a_modAtomPointerCnt;
	delCnt=a_delCnt;
	newRootsCnt=a_newRootsCnt;
	insertsCnt=a_insertsCnt;
}

Q_s_execution_finishedPackage::Q_s_execution_finishedPackage()
{
	modAtomPointerCnt=0;
	delCnt=0;
	newRootsCnt=0;
	insertsCnt=0;
}

Q_s_execution_finishedPackage::~Q_s_execution_finishedPackage()
{
}

void Q_s_execution_finishedPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeVaruint(modAtomPointerCnt);
	writer->writeVaruint(delCnt);
	writer->writeVaruint(newRootsCnt);
	writer->writeVaruint(insertsCnt);
}

bool Q_s_execution_finishedPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	{bool b; modAtomPointerCnt=reader->readVaruint(b);}
	{bool b; delCnt=reader->readVaruint(b);}
	{bool b; newRootsCnt=reader->readVaruint(b);}
	{bool b; insertsCnt=reader->readVaruint(b);}
	return true;
}

bool Q_s_execution_finishedPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	Q_s_execution_finishedPackage* p=(Q_s_execution_finishedPackage*)_package;
	if (!(getModAtomPointerCnt()==p->getModAtomPointerCnt())) return false;
	if (!(getDelCnt()==p->getDelCnt())) return false;
	if (!(getNewRootsCnt()==p->getNewRootsCnt())) return false;
	if (!(getInsertsCnt()==p->getInsertsCnt())) return false;
	return true;
}

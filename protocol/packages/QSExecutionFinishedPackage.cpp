#include <string.h>
#include "QSExecutionFinishedPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

QSExecutionFinishedPackage::QSExecutionFinishedPackage()
{
	modAtomPointerCnt=0;
	delCnt=0;
	newRootsCnt=0;
	insertsCnt=0;
	
	bnull_modAtomPointerCnt=true;
	bnull_delCnt=true;
	bnull_newRootsCnt=true;
	bnull_insertsCnt=true;
}

QSExecutionFinishedPackage::QSExecutionFinishedPackage(
			uint64_t a_modAtomPointerCnt,
			bool a_bnull_modAtomPointerCnt,
			uint64_t a_delCnt,
			bool a_bnull_delCnt,
			uint64_t a_newRootsCnt,
			bool a_bnull_newRootsCnt,
			uint64_t a_insertsCnt,
			bool a_bnull_insertsCnt
		)
{
	modAtomPointerCnt=a_modAtomPointerCnt;
	delCnt=a_delCnt;
	newRootsCnt=a_newRootsCnt;
	insertsCnt=a_insertsCnt;
	
	bnull_modAtomPointerCnt=a_bnull_modAtomPointerCnt;
	bnull_delCnt=a_bnull_delCnt;
	bnull_newRootsCnt=a_bnull_newRootsCnt;
	bnull_insertsCnt=a_bnull_insertsCnt;
};
			
void QSExecutionFinishedPackage::serializeW(PackageBufferWriter *writer)
{
	if (bnull_modAtomPointerCnt)
          writer->writeVaruintNull();
	else  writer->writeVaruint(modAtomPointerCnt);
	
	if (bnull_delCnt)
          writer->writeVaruintNull();
	else  writer->writeVaruint(delCnt);
	
	if (bnull_newRootsCnt)
          writer->writeVaruintNull();
	else  writer->writeVaruint(newRootsCnt);
	
	if (bnull_insertsCnt)
          writer->writeVaruintNull();
	else  writer->writeVaruint(insertsCnt);
}

bool  QSExecutionFinishedPackage::deserializeR(PackageBufferReader *reader)
{
	modAtomPointerCnt=reader->readVaruint(bnull_modAtomPointerCnt);
	delCnt=reader->readVaruint(bnull_delCnt);
	newRootsCnt=reader->readVaruint(bnull_newRootsCnt);
	insertsCnt=reader->readVaruint(bnull_insertsCnt);
	return true;
}

bool QSExecutionFinishedPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
 	QSExecutionFinishedPackage *pack=(QSExecutionFinishedPackage*)package;
 	
 	if ((bnull_modAtomPointerCnt!=pack->getBnull_modAtomPointerCnt())
		||(bnull_delCnt!=pack->getBnull_delCnt())
		||(bnull_newRootsCnt!=pack->getBnull_newRootsCnt())
		||(bnull_insertsCnt!=pack->getBnull_insertsCnt()))
		return false;

	if ((!bnull_modAtomPointerCnt) && (modAtomPointerCnt!=pack->getModAtomPointerCnt()))
		return false;
		
	if ((!bnull_delCnt) && (delCnt!=pack->getDelCnt()))
		return false;
 	
 	if ((!bnull_newRootsCnt) && (newRootsCnt=pack->getNewRootsCnt()))
		return false;
 	
 	if ((!bnull_insertsCnt) && (insertsCnt!=pack->getInsertsCnt()))
		return false;
 	
 	
   	return true;
 }



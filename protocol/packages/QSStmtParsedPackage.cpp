#include <string.h>
#include "QSStmtParsedPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

QSStmtParsedPackage::QSStmtParsedPackage()
{
	statement_id=0;
	params_cnt=0;
}

QSStmtParsedPackage::QSStmtParsedPackage(uint64_t a_statement_id, uint32_t a_params_cnt)
{
	statement_id =a_statement_id;
	params_cnt   =a_params_cnt;  
}
			
void QSStmtParsedPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint64(statement_id);
	writer->writeUint32(params_cnt);
}

bool  QSStmtParsedPackage::deserializeR(PackageBufferReader *reader)
{
	statement_id =reader->readUint64();
	params_cnt   =reader->readUint32();
	return true;
}

bool QSStmtParsedPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
 	QSStmtParsedPackage *pack=(QSStmtParsedPackage*)package;
 	
 	if (statement_id!=pack->getStatement_id())
 		return false;
 		
 	if (params_cnt!=pack->getParams_cnt())
 		return false;
 	
   	return true;
 }



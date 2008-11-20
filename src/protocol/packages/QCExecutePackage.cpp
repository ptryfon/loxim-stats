#include <string.h>
#include <protocol/packages/QCExecutePackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;

QCExecutePackage::QCExecutePackage()
{
	statement_id=0;
	flags=0;
	paramsCnt=0;
	params=NULL;
}

QCExecutePackage::QCExecutePackage(
			uint64_t a_statement_id,
			uint64_t a_flags, // kombinacja flag: STATEMENT_EXECUTE_FLAG_...
			uint32_t a_paramsCnt,
			uint64_t* a_params //tablica valueId - zostanie przejêta - tzn. zwolniona w destruktorze obiektu przez delete[] 
			)
{
	statement_id=a_statement_id;
	flags=a_flags;
	paramsCnt=a_paramsCnt;
	params=a_params;	
};

QCExecutePackage::~QCExecutePackage()
{
	if (params)
		delete[] params;	
};

			
void QCExecutePackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint64(statement_id);
	writer->writeUint64(flags);
	writer->writeUint32(paramsCnt);
	
	for(uint32_t i=0; i<paramsCnt; i++)
		writer->writeVaruint(params[i]);
}

bool  QCExecutePackage::deserializeR(PackageBufferReader *reader)
{
	statement_id	=reader->readUint64();
	flags			=reader->readUint64();	
	paramsCnt		=reader->readUint32();
	
	params=new uint64_t[paramsCnt];	
	bool null;	
	for(uint32_t i=0; i<paramsCnt; i++)
		params[i] = reader->readVaruint(null);
	
	return true;
}

bool QCExecutePackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
 	QCExecutePackage *pack=(QCExecutePackage*)package;
 	
 	if (statement_id!=pack->getStatement_id())
 		return false;
 		
 	if (flags!=pack->getFlags())
 		return false;
 		
 	if (paramsCnt!=pack->getParamsCnt())
 		return false;
 		
 	for (uint32_t i=0; i<paramsCnt; i++)
 	{
 		if ( params[i] != pack->getParams()[i] )
 			return false;
 	}
   	return true;
 }



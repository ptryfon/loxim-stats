#include <string.h>
#include <protocol/packages/QCStatementPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;

QCStatementPackage::QCStatementPackage()
{
	flags=0;
	statement=NULL;
}

QCStatementPackage::QCStatementPackage(uint64_t a_flags, char* a_statement)
{
	flags=a_flags;
	statement=new CharArray(strdup(a_statement));
}
		
//Nie kopiuje a_statement
QCStatementPackage::QCStatementPackage(uint64_t a_flags, CharArray* a_statement)
{
	flags=a_flags;
	statement=a_statement;
}
		
		
QCStatementPackage::~QCStatementPackage()
{
	if (statement!=NULL)
		delete statement;
}
			
void QCStatementPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint64(flags);
	writer->writeString(statement);
}

bool  QCStatementPackage::deserializeR(PackageBufferReader *reader)
{
	flags=reader->readUint64();
	statement=reader->readString();
	return true;
}

bool QCStatementPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
 	QCStatementPackage *pack=(QCStatementPackage *)package;
 	if (flags!=pack->getFlags())
 		return false;
 	
   	return CharArray::equal(statement,pack->getStatement());;
 }



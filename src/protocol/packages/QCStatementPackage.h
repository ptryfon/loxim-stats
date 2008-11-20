#ifndef QCSTATEMENTPACKAGE_H_
#define QCSTATEMENTPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_QCStatementPackage 64

#define STATEMENT_FLAG_EXECUTE		0x0001
#define STATEMENT_FLAG_READONLY	0x0002

#define STATEMENT_EXECUTE_FLAG_PREFER_DFS		0x0100
#define STATEMENT_EXECUTE_FLAG_PREFER_BSF		0x0200

namespace protocol{

class QCStatementPackage:public Package
{
	private:		
		uint64_t flags;
		CharArray *statement;
	
	public: 
		QCStatementPackage();
		
		//Kopiuje a_statement
		QCStatementPackage(uint64_t a_flags, char* a_statement);
		
		//Nie Kopiuje a_statement
		QCStatementPackage(uint64_t a_flags, CharArray* a_statement);
		
		
		virtual ~QCStatementPackage();
			
		uint8_t getPackageType(){return ID_QCStatementPackage;};
		
		uint64_t getFlags()
		{
			return flags;
		};
		
		CharArray *getStatement()
		{
			return statement;
		};
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*QCSTATEMENTPACKAGE_H_*/

#ifndef QSSTMTPARSEDPACKAGE_H_
#define QSSTMTPARSEDPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_QSStmtParsedPackage 65

namespace protocol{

class QSStmtParsedPackage:public Package
{
	private:
		uint64_t statement_id;
		uint32_t params_cnt;
	
	public: 
		QSStmtParsedPackage();
		QSStmtParsedPackage(uint64_t a_statement_id, uint32_t a_params_cnt);
		~QSStmtParsedPackage(){};
			
		uint8_t getPackageType(){return ID_QSStmtParsedPackage;};
		
		virtual bool equals(Package* package);
		
		uint64_t getStatement_id(){return statement_id;}
		
		uint32_t getParams_cnt(){return params_cnt;}
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*QSSTMTPARSEDPACKAGE_H_*/

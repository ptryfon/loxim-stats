#ifndef QCEXECUTEPACKAGE_H_
#define QCEXECUTEPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_QCExecutePackage 66

namespace protocol{

class QCExecutePackage:public Package
{
	private:		
		uint64_t statement_id;
		uint64_t flags; // kombinacja flag: STATEMENT_EXECUTE_FLAG_...
		uint32_t paramsCnt;
		uint64_t* params; //tablica valueId
		
	public: 
		QCExecutePackage();
		QCExecutePackage(
			uint64_t a_statement_id,
			uint64_t a_flags, // kombinacja flag: STATEMENT_EXECUTE_FLAG_...
			uint32_t a_paramsCnt,
			uint64_t* a_params //tablica valueId - zostanie przejêta - tzn. zwolniona w destruktorze obiektu przez delete[] 
			);
		~QCExecutePackage();
			
		uint8_t getPackageType(){return ID_QCExecutePackage;};
		
		uint64_t getStatement_id(){return statement_id;};
		uint64_t getFlags(){return flags;}; // kombinacja flag: STATEMENT_EXECUTE_FLAG_...
		uint32_t getParamsCnt(){return paramsCnt;};
		uint64_t* getParams(){return params;} //tablica valueId
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*QCEXECUTEPACKAGE_H_*/

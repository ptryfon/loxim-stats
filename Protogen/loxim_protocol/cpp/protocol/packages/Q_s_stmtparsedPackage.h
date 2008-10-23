#ifndef Q_S_STMTPARSEDPACKAGE_H_
#define Q_S_STMTPARSEDPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Q_s_stmtparsedPackage 65

class Q_s_stmtparsedPackage : public Package
{
	private:
		uint64_t statement_id;
		uint32_t paramsCnt;
	public:
		Q_s_stmtparsedPackage();
		~Q_s_stmtparsedPackage();
		Q_s_stmtparsedPackage(
			uint64_t a_statement_id,\
			uint32_t a_paramsCnt
			);

		virtual bool equals(Package* _package);

		uint64_t getStatement_id(){return statement_id;};
		void setStatement_id(uint64_t a_statement_id){statement_id=a_statement_id;};

		uint32_t getParamsCnt(){return paramsCnt;};
		void setParamsCnt(uint32_t a_paramsCnt){paramsCnt=a_paramsCnt;};


		uint8_t getPackageType(){return ID_Q_s_stmtparsedPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define Q_S_STMTPARSEDPACKAGE_H_*/

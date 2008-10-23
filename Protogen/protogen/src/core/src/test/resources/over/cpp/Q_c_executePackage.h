#ifndef Q_C_EXECUTEPACKAGE_H_
#define Q_C_EXECUTEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Q_c_executePackage 66

class Q_c_executePackage : public Package
{
	private:
		uint64_t statement_id;
		struct Statement_flagsMap flags;
		uint32_t paramsCnt;
		int64_t *params;
	public:
		Q_c_executePackage();
		~Q_c_executePackage();
		Q_c_executePackage(
			uint64_t a_statement_id,\
			struct Statement_flagsMap a_flags,\
			uint32_t a_paramsCnt,\
			int64_t *a_params
			);

		virtual bool equals(Package* _package);

		uint64_t getStatement_id(){return statement_id;};
		void setStatement_id(uint64_t a_statement_id){statement_id=a_statement_id;};

		struct Statement_flagsMap getFlags(){return flags;};
		void setFlags(struct Statement_flagsMap a_flags){flags=a_flags;};

		uint32_t getParamsCnt(){return paramsCnt;};
		void setParamsCnt(uint32_t a_paramsCnt){paramsCnt=a_paramsCnt;};

		int64_t* getParams(){return params;};
		void setParams(int64_t* a_params){params=a_params;};


		uint8_t getPackageType(){return ID_Q_c_executePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define Q_C_EXECUTEPACKAGE_H_*/

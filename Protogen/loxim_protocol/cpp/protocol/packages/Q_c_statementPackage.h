#ifndef Q_C_STATEMENTPACKAGE_H_
#define Q_C_STATEMENTPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Q_c_statementPackage 64

class Q_c_statementPackage : public Package
{
	private:
		struct Statement_flagsMap flags;
		CharArray* statement;
	public:
		Q_c_statementPackage();
		~Q_c_statementPackage();
		Q_c_statementPackage(
			struct Statement_flagsMap a_flags,\
			CharArray* a_statement
			);

		virtual bool equals(Package* _package);

		struct Statement_flagsMap getFlags(){return flags;};
		void setFlags(struct Statement_flagsMap a_flags){flags=a_flags;};

		CharArray* getStatement(){return statement;};
		void setStatement(CharArray* a_statement){statement=a_statement;};


		uint8_t getPackageType(){return ID_Q_c_statementPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define Q_C_STATEMENTPACKAGE_H_*/

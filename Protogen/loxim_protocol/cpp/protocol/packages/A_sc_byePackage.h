#ifndef A_SC_BYEPACKAGE_H_
#define A_SC_BYEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_A_sc_byePackage 3

class A_sc_byePackage : public Package
{
	private:
		enum Bye_reasonsEnum reason_code;
		CharArray* reason_description;
	public:
		A_sc_byePackage();
		~A_sc_byePackage();
		A_sc_byePackage(
			enum Bye_reasonsEnum a_reason_code,\
			CharArray* a_reason_description
			);

		virtual bool equals(Package* _package);

		enum Bye_reasonsEnum getReason_code(){return reason_code;};
		void setReason_code(enum Bye_reasonsEnum a_reason_code){reason_code=a_reason_code;};

		CharArray* getReason_description(){return reason_description;};
		void setReason_description(CharArray* a_reason_description){reason_description=a_reason_description;};


		uint8_t getPackageType(){return ID_A_sc_byePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define A_SC_BYEPACKAGE_H_*/

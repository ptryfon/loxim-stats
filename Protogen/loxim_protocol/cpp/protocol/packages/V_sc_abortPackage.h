#ifndef V_SC_ABORTPACKAGE_H_
#define V_SC_ABORTPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_V_sc_abortPackage 35

class V_sc_abortPackage : public Package
{
	private:
		enum Abort_reason_codesEnum reasonCode;
		CharArray* reasonString;
	public:
		V_sc_abortPackage();
		~V_sc_abortPackage();
		V_sc_abortPackage(
			enum Abort_reason_codesEnum a_reasonCode,\
			CharArray* a_reasonString
			);

		virtual bool equals(Package* _package);

		enum Abort_reason_codesEnum getReasonCode(){return reasonCode;};
		void setReasonCode(enum Abort_reason_codesEnum a_reasonCode){reasonCode=a_reasonCode;};

		CharArray* getReasonString(){return reasonString;};
		void setReasonString(CharArray* a_reasonString){reasonString=a_reasonString;};


		uint8_t getPackageType(){return ID_V_sc_abortPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define V_SC_ABORTPACKAGE_H_*/

#ifndef V_SC_FINISHEDPACKAGE_H_
#define V_SC_FINISHEDPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_V_sc_finishedPackage 34

class V_sc_finishedPackage : public Package
{
	private:
	public:
		V_sc_finishedPackage();
		~V_sc_finishedPackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_V_sc_finishedPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define V_SC_FINISHEDPACKAGE_H_*/

#ifndef A_SC_OKPACKAGE_H_
#define A_SC_OKPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_A_sc_okPackage 1

class A_sc_okPackage : public Package
{
	private:
	public:
		A_sc_okPackage();
		~A_sc_okPackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_A_sc_okPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define A_SC_OKPACKAGE_H_*/

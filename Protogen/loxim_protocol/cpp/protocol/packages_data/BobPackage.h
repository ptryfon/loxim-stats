#ifndef BOBPACKAGE_H_
#define BOBPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_BobPackage 15

class BobPackage : public Package
{
	private:
		CharArray* value;
	public:
		BobPackage();
		~BobPackage();
		BobPackage(
			CharArray* a_value
			);

		virtual bool equals(Package* _package);

		CharArray* getValue(){return value;};
		void setValue(CharArray* a_value){value=a_value;};


		uint8_t getPackageType(){return ID_BobPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define BOBPACKAGE_H_*/

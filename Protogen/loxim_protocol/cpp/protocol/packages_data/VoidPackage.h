#ifndef VOIDPACKAGE_H_
#define VOIDPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_VoidPackage 128

class VoidPackage : public Package
{
	private:
	public:
		VoidPackage();
		~VoidPackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_VoidPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define VOIDPACKAGE_H_*/

#ifndef UINT64PACKAGE_H_
#define UINT64PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Uint64Package 7

class Uint64Package : public Package
{
	private:
		uint64_t value;
	public:
		Uint64Package();
		~Uint64Package();
		Uint64Package(
			uint64_t a_value
			);

		virtual bool equals(Package* _package);

		uint64_t getValue(){return value;};
		void setValue(uint64_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Uint64Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define UINT64PACKAGE_H_*/

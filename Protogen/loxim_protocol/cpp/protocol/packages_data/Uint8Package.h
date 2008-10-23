#ifndef UINT8PACKAGE_H_
#define UINT8PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Uint8Package 1

class Uint8Package : public Package
{
	private:
		uint8_t value;
	public:
		Uint8Package();
		~Uint8Package();
		Uint8Package(
			uint8_t a_value
			);

		virtual bool equals(Package* _package);

		uint8_t getValue(){return value;};
		void setValue(uint8_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Uint8Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define UINT8PACKAGE_H_*/

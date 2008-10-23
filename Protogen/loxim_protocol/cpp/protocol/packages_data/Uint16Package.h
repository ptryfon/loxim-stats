#ifndef UINT16PACKAGE_H_
#define UINT16PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Uint16Package 3

class Uint16Package : public Package
{
	private:
		uint16_t value;
	public:
		Uint16Package();
		~Uint16Package();
		Uint16Package(
			uint16_t a_value
			);

		virtual bool equals(Package* _package);

		uint16_t getValue(){return value;};
		void setValue(uint16_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Uint16Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define UINT16PACKAGE_H_*/

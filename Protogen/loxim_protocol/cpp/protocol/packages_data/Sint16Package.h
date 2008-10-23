#ifndef SINT16PACKAGE_H_
#define SINT16PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Sint16Package 4

class Sint16Package : public Package
{
	private:
		int16_t value;
	public:
		Sint16Package();
		~Sint16Package();
		Sint16Package(
			int16_t a_value
			);

		virtual bool equals(Package* _package);

		int16_t getValue(){return value;};
		void setValue(int16_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Sint16Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define SINT16PACKAGE_H_*/

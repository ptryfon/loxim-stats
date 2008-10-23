#ifndef UINT32PACKAGE_H_
#define UINT32PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Uint32Package 5

class Uint32Package : public Package
{
	private:
		uint32_t value;
	public:
		Uint32Package();
		~Uint32Package();
		Uint32Package(
			uint32_t a_value
			);

		virtual bool equals(Package* _package);

		uint32_t getValue(){return value;};
		void setValue(uint32_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Uint32Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define UINT32PACKAGE_H_*/

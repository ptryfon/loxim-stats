#ifndef SINT32PACKAGE_H_
#define SINT32PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Sint32Package 6

class Sint32Package : public Package
{
	private:
		int32_t value;
	public:
		Sint32Package();
		~Sint32Package();
		Sint32Package(
			int32_t a_value
			);

		virtual bool equals(Package* _package);

		int32_t getValue(){return value;};
		void setValue(int32_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Sint32Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define SINT32PACKAGE_H_*/

#ifndef SINT64PACKAGE_H_
#define SINT64PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Sint64Package 8

class Sint64Package : public Package
{
	private:
		int64_t value;
	public:
		Sint64Package();
		~Sint64Package();
		Sint64Package(
			int64_t a_value
			);

		virtual bool equals(Package* _package);

		int64_t getValue(){return value;};
		void setValue(int64_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Sint64Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define SINT64PACKAGE_H_*/

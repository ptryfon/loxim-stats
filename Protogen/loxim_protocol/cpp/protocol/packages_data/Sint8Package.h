#ifndef SINT8PACKAGE_H_
#define SINT8PACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Sint8Package 2

class Sint8Package : public Package
{
	private:
		int8_t value;
	public:
		Sint8Package();
		~Sint8Package();
		Sint8Package(
			int8_t a_value
			);

		virtual bool equals(Package* _package);

		int8_t getValue(){return value;};
		void setValue(int8_t a_value){value=a_value;};


		uint8_t getPackageType(){return ID_Sint8Package;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define SINT8PACKAGE_H_*/

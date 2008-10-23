#ifndef BOOLPACKAGE_H_
#define BOOLPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_BoolPackage 9

class BoolPackage : public Package
{
	private:
		bool value;
	public:
		BoolPackage();
		~BoolPackage();
		BoolPackage(
			bool a_value
			);

		virtual bool equals(Package* _package);

		bool getValue(){return value;};
		void setValue(bool a_value){value=a_value;};


		uint8_t getPackageType(){return ID_BoolPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define BOOLPACKAGE_H_*/

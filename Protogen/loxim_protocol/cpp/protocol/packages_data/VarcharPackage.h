#ifndef VARCHARPACKAGE_H_
#define VARCHARPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_VarcharPackage 16

class VarcharPackage : public Package
{
	private:
		CharArray* value;
	public:
		VarcharPackage();
		~VarcharPackage();
		VarcharPackage(
			CharArray* a_value
			);

		virtual bool equals(Package* _package);

		CharArray* getValue(){return value;};
		void setValue(CharArray* a_value){value=a_value;};


		uint8_t getPackageType(){return ID_VarcharPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define VARCHARPACKAGE_H_*/

#ifndef REFPACKAGE_H_
#define REFPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_RefPackage 134

class RefPackage : public Package
{
	private:
		uint64_t valueId;
	public:
		RefPackage();
		~RefPackage();
		RefPackage(
			uint64_t a_valueId
			);

		virtual bool equals(Package* _package);

		uint64_t getValueId(){return valueId;};
		void setValueId(uint64_t a_valueId){valueId=a_valueId;};


		uint8_t getPackageType(){return ID_RefPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define REFPACKAGE_H_*/

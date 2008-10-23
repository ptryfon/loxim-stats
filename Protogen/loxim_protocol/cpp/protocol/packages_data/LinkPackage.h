#ifndef LINKPACKAGE_H_
#define LINKPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_LinkPackage 129

class LinkPackage : public Package
{
	private:
		int64_t valueId;
	public:
		LinkPackage();
		~LinkPackage();
		LinkPackage(
			int64_t a_valueId
			);

		virtual bool equals(Package* _package);

		int64_t getValueId(){return valueId;};
		void setValueId(int64_t a_valueId){valueId=a_valueId;};


		uint8_t getPackageType(){return ID_LinkPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define LINKPACKAGE_H_*/

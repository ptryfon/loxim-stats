#ifndef STRUCTPACKAGE_H_
#define STRUCTPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "CollectionPackage.h"

namespace protocol{

#define ID_StructPackage 131

class StructPackage : public CollectionPackage
{
	private:
	public:
		StructPackage();
		~StructPackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_StructPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define STRUCTPACKAGE_H_*/

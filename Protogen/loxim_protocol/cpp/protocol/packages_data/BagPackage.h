#ifndef BAGPACKAGE_H_
#define BAGPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "CollectionPackage.h"

namespace protocol{

#define ID_BagPackage 132

class BagPackage : public CollectionPackage
{
	private:
	public:
		BagPackage();
		~BagPackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_BagPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define BAGPACKAGE_H_*/

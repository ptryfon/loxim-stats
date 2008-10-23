#ifndef SEQUENCEPACKAGE_H_
#define SEQUENCEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "CollectionPackage.h"

namespace protocol{

#define ID_SequencePackage 133

class SequencePackage : public CollectionPackage
{
	private:
	public:
		SequencePackage();
		~SequencePackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_SequencePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define SEQUENCEPACKAGE_H_*/

#ifndef EXTERNAL_REFPACKAGE_H_
#define EXTERNAL_REFPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_External_refPackage 135

class External_refPackage : public Package
{
	private:
		uint64_t valueId;
		uint64_t stamp;
	public:
		External_refPackage();
		~External_refPackage();
		External_refPackage(
			uint64_t a_valueId,\
			uint64_t a_stamp
			);

		virtual bool equals(Package* _package);

		uint64_t getValueId(){return valueId;};
		void setValueId(uint64_t a_valueId){valueId=a_valueId;};

		uint64_t getStamp(){return stamp;};
		void setStamp(uint64_t a_stamp){stamp=a_stamp;};


		uint8_t getPackageType(){return ID_External_refPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define EXTERNAL_REFPACKAGE_H_*/

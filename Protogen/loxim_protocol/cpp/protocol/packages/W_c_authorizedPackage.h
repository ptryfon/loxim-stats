#ifndef W_C_AUTHORIZEDPACKAGE_H_
#define W_C_AUTHORIZEDPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_W_c_authorizedPackage 14

class W_c_authorizedPackage : public Package
{
	private:
	public:
		W_c_authorizedPackage();
		~W_c_authorizedPackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_W_c_authorizedPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define W_C_AUTHORIZEDPACKAGE_H_*/

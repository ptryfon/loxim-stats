#ifndef W_C_LOGINPACKAGE_H_
#define W_C_LOGINPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_W_c_loginPackage 13

class W_c_loginPackage : public Package
{
	private:
		enum Auth_methodsEnum auth_method;
	public:
		W_c_loginPackage();
		~W_c_loginPackage();
		W_c_loginPackage(
			enum Auth_methodsEnum a_auth_method
			);

		virtual bool equals(Package* _package);

		enum Auth_methodsEnum getAuth_method(){return auth_method;};
		void setAuth_method(enum Auth_methodsEnum a_auth_method){auth_method=a_auth_method;};


		uint8_t getPackageType(){return ID_W_c_loginPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define W_C_LOGINPACKAGE_H_*/

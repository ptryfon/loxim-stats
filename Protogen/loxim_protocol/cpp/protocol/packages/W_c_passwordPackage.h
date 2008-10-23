#ifndef W_C_PASSWORDPACKAGE_H_
#define W_C_PASSWORDPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_W_c_passwordPackage 15

class W_c_passwordPackage : public Package
{
	private:
		CharArray* login;
		CharArray* password;
	public:
		W_c_passwordPackage();
		~W_c_passwordPackage();
		W_c_passwordPackage(
			CharArray* a_login,\
			CharArray* a_password
			);

		virtual bool equals(Package* _package);

		CharArray* getLogin(){return login;};
		void setLogin(CharArray* a_login){login=a_login;};

		CharArray* getPassword(){return password;};
		void setPassword(CharArray* a_password){password=a_password;};


		uint8_t getPackageType(){return ID_W_c_passwordPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define W_C_PASSWORDPACKAGE_H_*/

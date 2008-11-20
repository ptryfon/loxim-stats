#ifndef WCPASSWORDPACKAGE_H_
#define WCPASSWORDPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_WCPasswordPackage 15

namespace protocol{

class WCPasswordPackage:public Package
{
	private:
		CharArray *login;
		CharArray *password;
	
	
	public: 
		//Wersja dla stringów zakoñczonych znakiem pustym, Kopiuje bufory.
		WCPasswordPackage(char* a_login,char* a_password); 
		
		//Wersja dla stringów zakoñczonych znakiem pustym, Nie kopiuje buforów.
		WCPasswordPackage(CharArray* a_login, CharArray* a_password);
		
		WCPasswordPackage();
		
		~WCPasswordPackage();
			
		uint8_t getPackageType(){return ID_WCPasswordPackage;};
		CharArray *getLogin(){return login;};
		CharArray *getPassword(){return login;}
		
		virtual bool equals(Package* package);  
	
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};
}
#endif /*WCMODEPACKAGE_H_*/

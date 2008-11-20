#ifndef WCLOGINPACKAGE_H_
#define WCLOGINPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_WCLoginPackage 13

namespace protocol{

class WCLoginPackage:public Package
{
	private:
		uint64_t auth_mode;		
	
	public: 
		WCLoginPackage(uint64_t a_authmode);
		WCLoginPackage();
			
		uint8_t getPackageType(){return ID_WCLoginPackage;};
		uint64_t getAuth_mode(){return auth_mode;};  
		
		virtual bool equals(Package* package);
	
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};
}
#endif /*WCLOGINPACKAGE_H_*/

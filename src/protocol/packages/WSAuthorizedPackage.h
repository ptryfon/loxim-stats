#ifndef WSAUTHORIZEDPACKAGE_H_
#define WSAUTHORIZEDPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_WSAuthorizedPackage 14

namespace protocol{

class WSAuthorizedPackage:public Package
{
	private:		
	
	public: 
		WSAuthorizedPackage();
			
		uint8_t getPackageType(){return ID_WSAuthorizedPackage;};  
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*WSAUTHORIZEDPACKAGE_H_*/

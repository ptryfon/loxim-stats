#ifndef ASCPINGPACKAGE_H_
#define ASCPINGPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_ASCPingPackage 129

namespace protocol{

class ASCPingPackage:public Package
{
	private:		
	
	public: 
		ASCPingPackage();
			
		uint8_t getPackageType(){return ID_ASCPingPackage;};  
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};
}
#endif /*ASCPINGPACKAGE_H_*/

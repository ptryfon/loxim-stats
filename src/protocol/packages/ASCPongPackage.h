#ifndef ASCPONGPACKAGE_H_
#define ASCPONGPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_ASCPongPackage 130

namespace protocol{

class ASCPongPackage:public Package
{
	private:		
	
	public: 
		ASCPongPackage();
			
		uint8_t getPackageType(){return ID_ASCPongPackage;};
		
		virtual bool equals(Package* package);  
	
	protected:	
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*ASCPONGPACKAGE_H_*/

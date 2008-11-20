#ifndef ASCOKPACKAGE_H_
#define ASCOKPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_ASCOkPackage 1

namespace protocol{

class ASCOkPackage:public Package
{
	private:		
	
	public: 
		ASCOkPackage();
			
		uint8_t getPackageType(){return ID_ASCOkPackage;};
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*ASCOKPACKAGE_H_*/

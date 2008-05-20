#ifndef WCMODEPACKAGE_H_
#define WCMODEPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_WCModePackage 12

namespace protocol{

class WCModePackage:public Package
{
	private:
		uint64_t mode;		
	
	public: 
		WCModePackage(uint64_t a_mode);
		WCModePackage(){mode=0;};
			
		uint8_t getPackageType(){return ID_WCModePackage;};
		uint64_t getMode(){return mode;};
		
		virtual bool equals(Package* package);
		
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};
}
#endif /*WCMODEPACKAGE_H_*/

#ifndef VSCFINISHEDPACKAGE_H_
#define VSCFINISHEDPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_VSCFinishedPackage 34

namespace protocol{

class VSCFinishedPackage:public Package
{
	private:		
	
	public: 
		VSCFinishedPackage();
			
		uint8_t getPackageType(){return ID_VSCFinishedPackage;};
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*VSCFINISHEDPACKAGE_H_*/

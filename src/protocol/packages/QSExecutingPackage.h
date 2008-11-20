#ifndef QSEXECUTINGPACKAGE_H_
#define QSEXECUTINGPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_QSExecutingPackage 67

namespace protocol{

class QSExecutingPackage:public Package
{
	private:		
	
	public: 
		QSExecutingPackage();
			
		uint8_t getPackageType(){return ID_QSExecutingPackage;};
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*QSEXECUTINGPACKAGE_H_*/

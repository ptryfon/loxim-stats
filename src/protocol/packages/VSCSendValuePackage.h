#ifndef VSCSENDVALUEPACKAGE_H_
#define VSCSENDVALUEPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"
#include "data/DataPart.h"

#define ID_VSCSendValuePackage 33

#define SEND_VALUE_FLAG_TO_BE_CONTINUED	0x01

namespace protocol{

class VSCSendValuePackage:public Package
{
	private:	
		uint64_t 	valueId;
		uint8_t	 	flags;
		DataPart* 	data;
	
	public: 
		VSCSendValuePackage();
		~VSCSendValuePackage();
		
		//Przkazane dane "data" nie s± kopiowane - a nastêpnie s± zwalniane w destruktorze pakietu
		VSCSendValuePackage(uint64_t valueId, uint8_t flags, DataPart* data);
			
		uint8_t getPackageType(){return ID_VSCSendValuePackage;};
		
		virtual bool equals(Package* package);
		
		uint64_t 	getValueId(){return valueId;};
		uint8_t	 	getFlags(){return flags;};
		DataPart* 	getData(){return data;};
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*VSCSENDVALUEPACKAGE_H_*/

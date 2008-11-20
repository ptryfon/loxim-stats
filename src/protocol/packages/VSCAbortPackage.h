#ifndef VSCABORTPACKAGE_H_
#define VSCABORTPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_VSCAbortPackage 35

namespace protocol{

class VSCAbortPackage:public Package
{
	private:		
		uint32_t reasonCode;
		CharArray *reasonString;
		
	public: 
		VSCAbortPackage();
		VSCAbortPackage(uint32_t a_reasonCode, CharArray* a_reasonString);
		VSCAbortPackage(uint32_t a_reasonCode, char* a_reasonString);
			
		uint8_t getPackageType(){return ID_VSCAbortPackage;};
		
		uint32_t getReasonCode(){return reasonCode;};
		CharArray *getReasonString(){return reasonString;};
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*VSCABORTPACKAGE_H_*/

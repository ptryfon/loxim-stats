#ifndef ASCBYEPACKAGE_H_
#define ASCBYEPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_ASCByePackage 3

namespace protocol{

class ASCByePackage:public Package
{
	private:		
		CharArray* reason;
		
	public: 
		/**Kopiuje dany string*/
		ASCByePackage(char* a_reason);
		
		/**Nie kopiuje danego opiektu*/
		ASCByePackage(CharArray* a_reason);
		ASCByePackage(){reason=NULL;};
		~ASCByePackage();
		
		CharArray* getReason(){return reason;};
			
	public:
		 uint8_t getPackageType(){return ID_ASCByePackage;};
		 
		 bool equals(Package* package);  
		 
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*ASCBYEPACKAGE_H_*/

#ifndef ASCSETOPTPACKAGE_H_
#define ASCSETOPTPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_ASCSetOptPackage 131

namespace protocol{

class ASCSetOptPackage:public Package
{
	private:		
		CharArray* key;
		CharArray* value;
		
	
	public: 
		/**Robi kopiê przekazanych napisów (zakoñczonych znakiem /0)*/
		ASCSetOptPackage(char* a_key, char* a_value);
		
		/**Nie robi kopi przekazanych obiektów*/
		ASCSetOptPackage(CharArray *a_key, CharArray* a_value);
		ASCSetOptPackage(){key=NULL; value=NULL;};
		~ASCSetOptPackage();
		
		uint8_t getPackageType(){return ID_ASCSetOptPackage;};
		
		CharArray* getKey(){return key;};
		CharArray* getValue(){return value;};
		
		virtual bool equals(Package* package);  
	
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};
}
#endif /*ASCSETOPTPACKAGE_H_*/

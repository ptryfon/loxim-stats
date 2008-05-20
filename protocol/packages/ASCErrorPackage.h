#ifndef ASCERRORPACKAGE_H_
#define ASCERRORPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_ASCErrorPackage 2

namespace protocol{

class ASCErrorPackage:public Package
{
	private:		
		uint32_t error_code;
		uint64_t object_id;
		bool	 object_null;
		CharArray *reason;
		uint32_t   line_nb;
		uint32_t   char_nb;
			
	public: 
		ASCErrorPackage(\
			uint32_t a_error_code,\
			uint64_t a_object_id,\
			bool	 a_object_null,\
			CharArray* 	 a_reason,\
			uint32_t a_line_nb,\
			uint32_t a_char_nb);
			
		ASCErrorPackage(\
			uint32_t a_error_code,\
			uint64_t a_object_id,\
			bool	 a_object_null,\
			char* 	 a_reason,\
			uint32_t a_line_nb,\
			uint32_t a_char_nb);
			
		ASCErrorPackage();
		~ASCErrorPackage();
		
		uint8_t getPackageType(){return ID_ASCErrorPackage;};
		
		virtual bool equals(Package* package);
				
		uint32_t getError_code(){return error_code;};
		uint64_t getObject_id(){return object_id;};
		bool	 getObject_null(){return object_null;};
		CharArray* getReason(){return reason;};
		uint32_t   getLine_nb(){return line_nb;};
		uint32_t   getChar_nb(){return char_nb;};
	
	 protected: 
	 	void serializeW(PackageBufferWriter *writter);
	 	bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*ASCERRORPACKAGE_H_*/

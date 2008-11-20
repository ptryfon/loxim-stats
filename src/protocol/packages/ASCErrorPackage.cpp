#include <string.h>
#include <stdint.h>
#include <protocol/packages/ASCErrorPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;
			

ASCErrorPackage::ASCErrorPackage(uint32_t a_error_code,\
			uint64_t a_object_id,\
			bool	 a_object_null,\
			CharArray* 	 a_reason,\
			uint32_t a_line_nb,\
			uint32_t a_char_nb)
{
	error_code=a_error_code;
	object_id=a_object_id;
	object_null=a_object_null;
	reason=a_reason;
	line_nb=a_line_nb;
	char_nb=a_char_nb;
}

ASCErrorPackage::ASCErrorPackage(uint32_t a_error_code,\
			uint64_t a_object_id,\
			bool	 a_object_null,\
			char* 	 a_reason,\
			uint32_t a_line_nb,\
			uint32_t a_char_nb)
{
	error_code=a_error_code;
	object_id=a_object_id;
	object_null=a_object_null;
	reason=new CharArray(strdup(a_reason));
	line_nb=a_line_nb;
	char_nb=a_char_nb;
}
			
void ASCErrorPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint32(error_code);
	if(object_null)
		writer->writeVaruintNull();
	else 
		writer->writeVaruint(object_id);	
	writer->writeSstring(reason);
	writer->writeInt32(line_nb);
	writer->writeInt32(char_nb);
}

bool  ASCErrorPackage::deserializeR(PackageBufferReader *reader)
{
	error_code=reader->readUint32();
	object_id=reader->readVaruint(object_null);
	reason=reader->readSstring();
	line_nb=reader->readInt32();
	char_nb=reader->readInt32();
	return true;
}

ASCErrorPackage::ASCErrorPackage()
{
	error_code=0;
	object_id=0;
	reason=NULL;
	line_nb=0;
	char_nb=0;
}


ASCErrorPackage::~ASCErrorPackage()
{
	if (reason)
		delete reason;
}


bool ASCErrorPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 	
 	if(!CharArray::equal(((ASCErrorPackage*)package)->getReason(),getReason()))
 		return false;
 		
 	if (error_code!= ((ASCErrorPackage*)package)->getError_code())
  		return false;
  		
  	if (line_nb!= ((ASCErrorPackage*)package)->getLine_nb())
  		return false; 		
  		
  	if (char_nb!= ((ASCErrorPackage*)package)->getChar_nb())
  		return false;
  		
  	return true;
 }


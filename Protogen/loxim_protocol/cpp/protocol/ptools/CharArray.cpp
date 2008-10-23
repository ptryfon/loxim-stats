#include "CharArray.h"

#include <string.h>
#include <stdlib.h>

using namespace protocol;

CharArray::~CharArray()
{
	if (buffor) 
		free(buffor);
}

CharArray::CharArray(char* a_buffor, uint32_t a_size)
{
	buffor=a_buffor;
	size=a_size;
}		

CharArray::CharArray(char* a_buffor)
{	
	buffor=a_buffor;
	size=strlen(a_buffor);
}


bool CharArray::equals(CharArray *ca)
{
	if (!ca)
		return false;
		
	if (getSize()!=ca->getSize())
		return false;
		
	return memcmp(ca->getBuffor(),getBuffor(),getSize())==0;
}

bool CharArray::equal(CharArray *a1, CharArray *a2)
{
	if (a1==a2)
		return true;
	
	if ((!a1)||(!a2))
		return false;
		
	return a1->equals(a2);
}

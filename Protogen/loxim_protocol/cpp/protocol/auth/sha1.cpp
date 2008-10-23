#include <stdlib.h>
#include <string.h> 
#include "sha1.h"
#include <openssl/sha.h>

#include "../ptools/CharArray.h"

using namespace protocol;

char* protocol::sha1(char* bytes)
{
	return sha1( bytes, strlen(bytes) );
}

char* protocol::sha1(CharArray* bytes)
{
	return sha1( bytes->getBuffor(), bytes->getSize() );
}

char* sha1(const char* bytes, int len)
{
	unsigned char* md=(unsigned char*)malloc(len);
	SHA1((const unsigned char*)bytes,len,md);
	return (char*)md;
}

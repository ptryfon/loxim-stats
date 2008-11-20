#ifndef NOSHA

#ifndef SHA1_
#define SHA1_

#include <protocol/ptools/CharArray.h>

namespace protocol{

/**
 * Liczy sha1 dla danego ciagu bajtów zakoñczonego znakiem 0.
 * Znak 0 nie jest liczony do zwracanej sumy. 
 * 
 * Zwraca tablicê 20 bajtów. 
 */
char* sha1(char* bytes);

char* sha1(CharArray* bytes);

/**
 * Zwraca tablicê 20 bajtów. 
 */
char* sha1(char* bytes, int len);

}

#endif /*SHA1_*/

#endif /*NOSHA*/

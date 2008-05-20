#ifndef _AUTHPASSMYSQL_H_
#define _AUTHPASSMYSQL_H_

#include "../ptools/CharArray.h"

namespace protocol{

/*Funkcja przygotowuj±ca skrót has³a na podstawie sha1 has³a (dla serwera)
 * 
 * Zarówno sha1_password jak i salt powinny mieæ po 20 bajtów. 
 * 
 * */
CharArray* encodePasswordFromShaP(char *sha1_password,char* salt);

/*Funkcja przygotowuj±ca skrót has³a na podstawie has³a (dla klienta)
 * 
 * Has³o powinno byæ prawid³owym ³ancuchem zakoñczonym znakiem "0",
 * a selt - tablic± 20 bajtów.  
 * 
 * */
CharArray* encodePassword(CharArray *password,char* salt);

}

#endif /*_AUTHPASSMYSQL_H_*/

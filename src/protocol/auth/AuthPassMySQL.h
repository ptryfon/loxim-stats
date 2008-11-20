#ifndef _AUTHPASSMYSQL_H_
#define _AUTHPASSMYSQL_H_

#include "../ptools/CharArray.h"

namespace protocol{

/*Funkcja przygotowuj�ca skr�t has�a na podstawie sha1 has�a (dla serwera)
 * 
 * Zar�wno sha1_password jak i salt powinny mie� po 20 bajt�w. 
 * 
 * */
CharArray* encodePasswordFromShaP(char *sha1_password,char* salt);

/*Funkcja przygotowuj�ca skr�t has�a na podstawie has�a (dla klienta)
 * 
 * Has�o powinno by� prawid�owym �ancuchem zako�czonym znakiem "0",
 * a selt - tablic� 20 bajt�w.  
 * 
 * */
CharArray* encodePassword(CharArray *password,char* salt);

}

#endif /*_AUTHPASSMYSQL_H_*/

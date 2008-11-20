#ifndef CONSTANTS_H_
#define CONSTANTS_H_

//Numerki og�lne

#define LOXIM_VERSION_MAJOR 0
#define LOXIM_VERSION_MINOR 1

#define PROTO_VERSION_MAJOR 2
#define PROTO_VERSION_MINOR 0

//================Tryby transmisji i dzia�ania protoko�u ======================
  //dost�pny SSL
#define PROTO_MODE_F_SSL   0x0001
  //oblikatoryjny SSL
#define PROTO_MODE_F_O_SSL 0x0002
  //dost�pny ZLIB
#define PROTO_MODE_F_ZLIB  0x0004
  
//===================== Tryby przetwarzania zapyta� ===========================

#define PROTO_MODEQ_F_AUTOCOMMIT    	0x0010
#define PROTO_MODEQ_F_OPTIMALIZATION   0x0020

//==================== Metody autoryzacji ======================================

#define PROTO_AM_TRUST  0x0001
#define PROTO_AM_MYSQL5 0x0002


#endif /*CONSTANTS_H_*/

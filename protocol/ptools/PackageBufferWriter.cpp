#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "PackageBufferWriter.h"
#include "Endianness.h"

#define DEFAULT_BUFFER_SIZE 4096

using namespace protocol;

PackageBufferWriter::PackageBufferWriter()
{
  stringBufferHead=new StringBufferStack(NULL,DEFAULT_BUFFER_SIZE);
  appendStr("xxxxx",5); //5 bajt�w zerezerwowane na pocz�tku na typ pakietu i jego d�ugo��
  size=0;
}

PackageBufferWriter::~PackageBufferWriter()
{
	if(stringBufferHead!=NULL)
		delete stringBufferHead;		
}

/**
 * Pobiera aktualny bufor utworzony przez klas� (jako wska�nik
 * do pami�ci zawartej w klasie). Automatycznie wywo�uje flush.
 * Nie zwraca 5 bajt�w nag��wka pakietu.  
 */
const char* PackageBufferWriter::getBuffer()
{
	flush(); 
	if(stringBufferHead!=NULL)
		return stringBufferHead->getBuffer()+5;
	else return "";
}

/**
 * Pobiera aktualny bufor utworzony przez klas� (jako wska�nik
 * do pami�ci zawartej w klasie). Automatycznie wywo�uje flush. 
 */
const char* PackageBufferWriter::getPackageBuffer(uint8_t packageType)
{
	flush(); 
	if(stringBufferHead!=NULL){
		char* buff=stringBufferHead->getBuffer();
		buff[0]=(char)packageType;
		*((uint32_t*)(buff+1))=ntohl(getSize());
		return buff;
	}
	else return "";
}
	
/**
 * Zwraca aktualn� d�ugo�� (w bajtach) utworzonego bufora. 
 */
int PackageBufferWriter::getSize()
{
	return size;
}
 
	
/**
 * Powyduje uporz�dkowanie wszystkich fragment�w
 * i uformowanie ich w bufor wynikowy, a nast�pnie
 * zwolnienie pomi�ci z tych fragment�w pomocniczych.  
 * */
void PackageBufferWriter::flush()
{
	if(stringBufferHead!=NULL)
	{
		StringBufferStack *ret=new StringBufferStack(NULL,size+5);
		StringBufferStack *iterator=stringBufferHead;
		while (iterator!=NULL)
		{
			ret->appendStrEnd(iterator->getBuffer(),iterator->getSize());
			iterator=iterator->getNext();
		};
		delete stringBufferHead;
		stringBufferHead=ret;
	}
}

	
/**
 * Czy�ci zupe�nie bufor i wszystkie zmienne pomocnicze.
 * W praktyce oznacza rozpocz�cie przygotowywania nowej paczki.
 */
void PackageBufferWriter::clear()
{
	if(stringBufferHead!=NULL)
	{
		delete stringBufferHead;
		stringBufferHead=NULL;
	}
	stringBufferHead=new StringBufferStack(NULL,DEFAULT_BUFFER_SIZE);
    appendStr("xxxxx",5); //5 bajt�w zerezerwowane na pocz�tku na typ pakietu i jego d�ugo��
    size=0;  
}


//================= TYPY CA�KOWITOLICZBOWE =============================
/**
 * Zwraca liczb� bajt�w zapisanych w buforze
 */
uint32_t PackageBufferWriter::writeInt8(const int8_t value)
{
	appendStr((char*)&value,1);
	return 1;
}

uint32_t PackageBufferWriter::writeInt16(const int16_t value)
{
	int16_t res=htons(value);
	appendStr((char*)&res,2);
	return 2;
}

uint32_t PackageBufferWriter::writeInt32(const int32_t value)
{
	int32_t res=htonl(value);
	appendStr((char*)&res,4);
	return 4;
}

uint32_t PackageBufferWriter::writeInt64(const int64_t value)
{
	int64_t res=htonll(value);
	appendStr((char*)&res,8);
	return 8;
}

uint32_t PackageBufferWriter::writeUint8(const uint8_t value)
{
	appendStr((char*)&value,1);
	return 1;
} 

uint32_t PackageBufferWriter::writeUint16(const uint16_t value)
{
	uint16_t res=htons(value);
	appendStr((char*)&res,2);
	return 2;
}
uint32_t PackageBufferWriter::writeUint32(const uint32_t value)
{
	uint32_t res=htonl(value);
	appendStr((char*)&res,4);
	return 4;
}
uint32_t PackageBufferWriter::writeUint64(const uint64_t value)
{
	uint64_t res=htonll(value);
	appendStr((char*)&res,8);
	return 8;
}
	
uint32_t PackageBufferWriter::writeVaruint(const uint64_t value)
{
	if(value<250)
	{		
		appendStr((char*)&value,1);
		return 1;
	}
	if(value<(1<<16)) //Mie�ci si� w dw�ch bajtach
	{
		uint16_t res=htons((uint16_t)value);
		uint8_t byte=251;
		appendStr((char*)&byte,1);
		appendStr((char*)&res,2);
		return 3; 
	}
	if(value<(1l<<32)) //Mie�ci si� w 4 bajtach
	{
		uint32_t res=htonl((uint32_t)value);
		uint8_t byte=252;
		appendStr((char*)&byte,1);
		appendStr((char*)&res,4);
		return 5; 
	}
	//A wi�c musi si� zmie�ci� w 8 bajtach
	uint64_t res=htonl((uint64_t)value);
	uint8_t byte=253;
	appendStr((char*)&byte,1);
	appendStr((char*)&res,8);
	return 9;		
}

uint32_t PackageBufferWriter::writeVaruintNull()
{
		uint8_t byte=250;
		appendStr((char*)&byte,1);
		return 1;
}
//==================== Zmiennoprzecinkowe ===============================

uint32_t PackageBufferWriter::writeDouble(double value)
{
	double res=htond(value);
	appendStr((char*)&res,8);
	return 8;	
}
	
//======================== Napisy ========================================
   
uint32_t PackageBufferWriter::writeSstring(char* str,int16_t a_size/*=-1*/)
{
	if(size>=250){
		//ERROR - nie prawid�owe u�ycie funkcji
	}
	return writeString(str,a_size==-1?strlen(str):a_size);
}

uint32_t PackageBufferWriter::writeString(char* str, int64_t a_size/*=-1*/)
{
	uint32_t sRet=(a_size==-1?strlen(str):a_size);
	sRet+=writeVaruint(sRet);
	appendStr(str,a_size);
	return sRet;	
}

uint32_t PackageBufferWriter::writeString(CharArray *ca)
{
	return writeString(ca->getBuffor(),ca->getSize());
}

uint32_t PackageBufferWriter::writeSstring(CharArray *ca)
{
	return writeSstring(ca->getBuffor(),ca->getSize());
}


//========================================================================

uint32_t PackageBufferWriter::writeBool(bool value)
{
	return writeInt8(value?1:0);
}

//=========================== Wewn�trzne==================================

void  PackageBufferWriter::appendStr(char* str, int64_t a_size)
{
	if(!stringBufferHead->appendStr(str,a_size)) //Je�li nie uda si� doda� do bie��cego bufora
	{ //Dodajemy do nowego bufora
		StringBufferStack *sbs=new StringBufferStack(stringBufferHead,(a_size<DEFAULT_BUFFER_SIZE/2)?DEFAULT_BUFFER_SIZE:a_size);	
		sbs->appendStr(str,a_size);	
		stringBufferHead=sbs;
	};
	size+=a_size;
}

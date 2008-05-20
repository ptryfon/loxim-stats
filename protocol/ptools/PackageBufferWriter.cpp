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
  appendStr("xxxxx",5); //5 bajtów zerezerwowane na pocz±tku na typ pakietu i jego d³ugo¶æ
  size=0;
}

PackageBufferWriter::~PackageBufferWriter()
{
	if(stringBufferHead!=NULL)
		delete stringBufferHead;		
}

/**
 * Pobiera aktualny bufor utworzony przez klasê (jako wska¼nik
 * do pamiêci zawartej w klasie). Automatycznie wywo³uje flush.
 * Nie zwraca 5 bajtów nag³ówka pakietu.  
 */
const char* PackageBufferWriter::getBuffer()
{
	flush(); 
	if(stringBufferHead!=NULL)
		return stringBufferHead->getBuffer()+5;
	else return "";
}

/**
 * Pobiera aktualny bufor utworzony przez klasê (jako wska¼nik
 * do pamiêci zawartej w klasie). Automatycznie wywo³uje flush. 
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
 * Zwraca aktualn± d³ugo¶æ (w bajtach) utworzonego bufora. 
 */
int PackageBufferWriter::getSize()
{
	return size;
}
 
	
/**
 * Powyduje uporz±dkowanie wszystkich fragmentów
 * i uformowanie ich w bufor wynikowy, a nastêpnie
 * zwolnienie pomiêci z tych fragmentów pomocniczych.  
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
 * Czy¶ci zupe³nie bufor i wszystkie zmienne pomocnicze.
 * W praktyce oznacza rozpoczêcie przygotowywania nowej paczki.
 */
void PackageBufferWriter::clear()
{
	if(stringBufferHead!=NULL)
	{
		delete stringBufferHead;
		stringBufferHead=NULL;
	}
	stringBufferHead=new StringBufferStack(NULL,DEFAULT_BUFFER_SIZE);
    appendStr("xxxxx",5); //5 bajtów zerezerwowane na pocz±tku na typ pakietu i jego d³ugo¶æ
    size=0;  
}


//================= TYPY CA£KOWITOLICZBOWE =============================
/**
 * Zwraca liczbê bajtów zapisanych w buforze
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
	if(value<(1<<16)) //Mie¶ci siê w dwóch bajtach
	{
		uint16_t res=htons((uint16_t)value);
		uint8_t byte=251;
		appendStr((char*)&byte,1);
		appendStr((char*)&res,2);
		return 3; 
	}
	if(value<(1l<<32)) //Mie¶ci siê w 4 bajtach
	{
		uint32_t res=htonl((uint32_t)value);
		uint8_t byte=252;
		appendStr((char*)&byte,1);
		appendStr((char*)&res,4);
		return 5; 
	}
	//A wiêc musi siê zmie¶ciæ w 8 bajtach
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
		//ERROR - nie prawid³owe u¿ycie funkcji
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

//=========================== Wewnêtrzne==================================

void  PackageBufferWriter::appendStr(char* str, int64_t a_size)
{
	if(!stringBufferHead->appendStr(str,a_size)) //Je¶li nie uda siê dodaæ do bie¿±cego bufora
	{ //Dodajemy do nowego bufora
		StringBufferStack *sbs=new StringBufferStack(stringBufferHead,(a_size<DEFAULT_BUFFER_SIZE/2)?DEFAULT_BUFFER_SIZE:a_size);	
		sbs->appendStr(str,a_size);	
		stringBufferHead=sbs;
	};
	size+=a_size;
}

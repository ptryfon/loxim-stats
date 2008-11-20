#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <protocol/ptools/PackageBufferReader.h>
#include <protocol/ptools/Endianness.h>

using namespace protocol;

PackageBufferReader::PackageBufferReader(const char* a_buffer, uint32_t a_size)
{
	buffer=a_buffer;
	pos=0;
	size=a_size;
	hasError=false;
}

PackageBufferReader::PackageBufferReader(const char* a_package)
{
	size=ntohl(*((int32_t*)(a_package+1)));
	pos=0;
	buffer=a_package+5;
	hasError=false;
}
		
		
PackageBufferReader::~PackageBufferReader()
{
}


int8_t PackageBufferReader::readInt8()
{
	if(!available_bytes(1)) return 0;
	int8_t res=*((int8_t*)(buffer+pos));
	pos++;
	return res;	
}

int16_t PackageBufferReader::readInt16()
{
	if(!available_bytes(2)) return 0;
	int16_t res=*((int16_t*)(buffer+pos));
	pos+=2;
	return ntohs(res);
}

int32_t PackageBufferReader::readInt32()
{
	if(!available_bytes(4)) return 0;
	int32_t res=*((int32_t*)(buffer+pos));
	pos+=4;
	return ntohl(res);
}

int64_t PackageBufferReader::readInt64()
{
	if(!available_bytes(8)) return 0;
	
	int64_t res=*((int64_t*)(buffer+pos));
	pos+=8;
	return ntohll(res);
}
	
uint8_t PackageBufferReader::readUint8()
{
	if(!available_bytes(1)) return 0;
	
	uint8_t res=*((uint8_t*)(buffer+pos));
	pos++;
	return res;
}

uint16_t PackageBufferReader::readUint16()
{
	if(!available_bytes(2)) return 0;
	
	uint16_t res=*((uint16_t*)(buffer+pos));
	pos+=2;
	return ntohs(res);
}

uint32_t PackageBufferReader::readUint32()
{
	if(!available_bytes(4)) return 0;
	
	uint32_t res=*((uint32_t*)(buffer+pos));
	pos+=4;
	return ntohl(res);
}

uint64_t PackageBufferReader::readUint64()
{
	if(!available_bytes(8)) return 0;
	uint64_t res=*((uint64_t*)(buffer+pos));
	pos+=8;
	return ntohll(res);
}

		
int64_t PackageBufferReader::readVaruint(bool &null)
{
	null=false;
	if(!available_bytes(1)) return 0;
	uint8_t first=readUint8();
	if (first<250)
		return first;
	if (first==250)
	{
		null=true;
		return 0;
	};
	switch(first)
	{
		case 251:
			return readUint16();
		case 252:
			return readUint32();
		case 253:
			return readUint64();
		default: 
			markError();//Nie znana warto¶æ wprowadzaj±ca
			return 0;
	};
}

char* PackageBufferReader::readSstring(uint32_t &a_size)
{
	if(!available_bytes(1)) return 0;
	a_size=readUint8();	
	if (a_size<250)
	{
	   if(!available_bytes(a_size)) 
	   		return 0;
	   	char* res=(char*)malloc((uint8_t)a_size+1);
	   	memcpy(res,buffer+pos,(uint8_t)a_size);
	   	res[a_size]=0;
	   	pos+=a_size;
	   	return res;
	}else
	{  //Short string musi mieæ mniej ni¿ 250 znaków
		if (a_size!=250) 
			markError(); //250 oznacza NULL'a
		a_size=0;
		return NULL;
	}	
}

char* PackageBufferReader::readString(uint32_t &a_size)
{
	bool null;
	a_size=readVaruint(null);	
	if(null)
	{
		a_size=0;
		return NULL;
	}	
	if(!available_bytes(a_size)) 
		return 0;
	char* res=(char*)malloc(a_size+1);
	memcpy(res,buffer+pos,a_size);
	res[a_size]=0;
	pos+=a_size;
	return res;		
}


/**
 * Alokuje (kopiuje) zwracan± warto¶æ - pamiêtaj by j± zwolniæ
 * */
CharArray* PackageBufferReader::readString()
{
	uint32_t size;
	char *res=readString(size);
	return res?new CharArray(res,size):NULL;
}

/**
 * Alokuje (kopiuje) zwracan± warto¶æ - pamiêtaj by j± zwolniæ
 * */
CharArray* PackageBufferReader::readSstring()
{
	uint32_t size;
	char *res=readSstring(size);
	return res?new CharArray(res,size):NULL;
}

				
double PackageBufferReader::readDouble()
{
	if(!available_bytes(8)) return 0;
	double res=*((double*)(buffer+pos));
	pos+=8;
	return ntohd(res);
}

bool PackageBufferReader::readBool()
{
	int8_t v=readInt8();
	switch(v)
	{
		case 0: return false;
		case 1: return true;
		default:
			markError();
			return false;
	};
}





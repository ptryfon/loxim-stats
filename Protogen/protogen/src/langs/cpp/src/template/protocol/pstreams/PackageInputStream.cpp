#include "PackageInputStream.h"
#include "../ptools/PackageHeader.h"
#include "../packages/PackagesFactory.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace protocol;
	
PackageInputStream::PackageInputStream(AbstractInputStream *a_inputStream)
{
	inputStream=a_inputStream;
}

PackageInputStream::~PackageInputStream()
{
}
	
Package *PackageInputStream::readPackage()
{
	return readPackage(0);
}

Package *PackageInputStream::readPackage(long timeout)
{
	char header[5];
	
	if(readNbytes(header,5, timeout)!=5)
	{
		//printf("Urwany header\n");
		return NULL;
	}
	
	uint32_t size=PackageHeader::getPackage_size(header);
	uint8_t  type=PackageHeader::getPackage_type(header);
	
	//printf("\n*Size pakietu to: %d, a a typ to: %d *\n",size,type);
	/*TODO: Sprawdziæ tutaj, czy wielko¶æ pakietu jest poni¿ej ograniczenia !!!*/
	
	char *data=(char*)malloc(size+5);
	
	if (!data) //Za ma³o pamiêci na bufor pakietu
		return NULL;
		
	if (readNbytes(data+5,size, timeout)!=size)
	{   /*Nie uda³o siê wczytaæ ca³ego pakietu*/
		free(data);   
		//printf("Za ma³o danych\n"); 
		return NULL;  
	}
	memcpy(data,header,5);
	Package* resPackage=PackagesFactory::createPackage(((uint8_t)type)/*, size*/, data);
	free(data);
	return resPackage;
}

unsigned int PackageInputStream::readNbytes(char* buffor,unsigned long int n, long timeout)
{
	unsigned int readlen=0;
	while(readlen<n)
	{
		int res;
		if (timeout)
			res = inputStream->read(buffor,readlen,n-readlen, timeout);
		else
			res = inputStream->read(buffor,readlen,n-readlen);
		//printf("\n - got bytes: %d - \n",res);
		if(res<=0)
			return readlen;
		readlen+=res;
	};	
	return n;
}

int PackageInputStream::getStatus()
{
	return inputStream->getStatus();
}

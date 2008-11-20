#include "Endianness.h"
#include <netinet/in.h>

using namespace protocol;

int64_t protocol::ntohll(int64_t src)
{
	if(htonl(1)==1) /*Wykrycie systemu o tej samej endianesowato¶ci*/
		return src;
		
	int64_t res=0;
	*(((int32_t*)(&res))+1)= htonl((int32_t)res);
	*((int32_t*)(&res))=htonl(*(((int32_t*)&res)+1));
	return src;
}

uint64_t protocol::ntohll(uint64_t src)
{	
	if(htonl(1)==1) /*Wykrycie systemu o tej samej endianesowato¶ci*/
		return src;
	int64_t res=0; 
	*(((int32_t*)(&res))+1)= htonl((int32_t)res);
	*((int32_t*)(&res))=htonl(*(((int32_t*)&res)+1));
	return src;
}

int64_t protocol::htonll(int64_t src)
{
	if(htonl(1)==1) /*Wykrycie systemu o tej samej endianesowato¶ci*/
		return src;
		
	int64_t res=0;
	*(((int32_t*)(&res))+1)= htonl((int32_t)res);
	*((int32_t*)(&res))=htonl(*(((int32_t*)&res)+1));
	return src;
}

uint64_t protocol::htonll(uint64_t src)
{
	if(htonl(1)==1) /*Wykrycie systemu o tej samej endianesowato¶ci*/
		return src;
	int64_t res=0;
	*(((int32_t*)(&res))+1)= htonl((int32_t)res);
	*((int32_t*)(&res))=htonl(*(((int32_t*)&res)+1));
	return src;
}

double protocol::ntohd(double src)
{
	if(htonl(1)==1) /*Wykrycie systemu o tej samej endianesowato¶ci*/
		return src;
	double res=0.0;
	*(((int32_t*)(&res))+1)= htonl((int32_t)res);
	*((int32_t*)(&res))=htonl(*(((int32_t*)&res)+1));
	return src;
}

double protocol::htond(double src)
{
	if(htonl(1)==1) /*Wykrycie systemu o tej samej endianesowato¶ci*/
		return src;
	double res=0.0;
	*(((int32_t*)(&res))+1)= htonl((int32_t)res);
	*((int32_t*)(&res))=htonl(*(((int32_t*)&res)+1));
	return src;
}


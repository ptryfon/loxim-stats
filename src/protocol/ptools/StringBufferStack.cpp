#include <stdlib.h>
#include <string.h>
#include <protocol/ptools/StringBufferStack.h>

using namespace protocol;

StringBufferStack::StringBufferStack(StringBufferStack *a_next,uint32_t a_length)
{
	next=a_next;
	used_start=0;
	used_end=0;
	buffer_len=a_length;
	buffer=(char*)malloc(a_length);
}

StringBufferStack::~StringBufferStack()
{
	free(buffer);
	delete next;
}


bool StringBufferStack::appendStr(char* str, uint32_t length)
{
	if(length<=buffer_len-used_end-used_start)
	{
		memcpy(buffer+used_start,str,length);
		used_start+=length;
		return true;
	}
	else
	 	return false;
}

bool StringBufferStack::appendStrEnd(char* str, uint32_t length)
{
	if(length<=buffer_len-used_start-used_end)
	{
		memcpy(buffer+buffer_len-used_end-length,str,length);
		used_end+=length;
		return true;
	}
	else
	 	return false;
}

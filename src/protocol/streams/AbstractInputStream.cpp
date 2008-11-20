#include <protocol/streams/AbstractInputStream.h>
#include <sys/time.h>

using namespace protocol;

int AbstractInputStream::read(char* buffor, unsigned long int length)
{
	return read(buffor,0,length);
}


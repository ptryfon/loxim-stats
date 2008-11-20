#include "AbstractOutputStream.h"

using namespace protocol;

int AbstractOutputStream::write(char* buffor,unsigned long int length)
{
	return write(buffor,0,length);
}

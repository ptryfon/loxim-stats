#include "PackageHeader.h"

#include <string.h>
#include <netinet/in.h>
#include "Package.h"
#include "PackageBufferWriter.h"
#include "PackageBufferReader.h"
#include "Endianness.h"

using namespace protocol;

uint8_t  PackageHeader::getPackage_type(char* buf)
{
	return buf[0];
}

uint32_t PackageHeader::getPackage_size(char* buf)
{
	return ntohl(*((int32_t*)(buf+1)));
}

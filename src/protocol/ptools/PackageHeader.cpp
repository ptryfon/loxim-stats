#include <protocol/ptools/PackageHeader.h>

#include <string.h>
#include <netinet/in.h>
#include <protocol/ptools/Package.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>
#include <protocol/ptools/Endianness.h>

using namespace protocol;

uint8_t  PackageHeader::getPackage_type(char* buf)
{
	return buf[0];
}

uint32_t PackageHeader::getPackage_size(char* buf)
{
	return ntohl(*((int32_t*)(buf+1)));
}

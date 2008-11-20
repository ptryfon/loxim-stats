#ifndef PACKAGEHEADER_H_
#define PACKAGEHEADER_H_

#include <stdint.h>
#include <protocol/ptools/Package.h>

namespace protocol{

class PackageHeader
{		
	public:
		static uint8_t  getPackage_type(char* buf);
		static uint32_t getPackage_size(char* buf);
};

}
#endif /*PACKAGEHEADER_H_*/

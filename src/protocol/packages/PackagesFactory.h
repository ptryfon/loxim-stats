#ifndef PACKAGESFACTORY_H_
#define PACKAGESFACTORY_H_

#include <stdint.h>
#include "../ptools/Package.h"

namespace protocol{

class PackagesFactory
{
	public:
		 static Package *createPackage(uint8_t type, uint32_t size, char* data); 
};
}
#endif /*PACKAGESFACTORY_H_*/

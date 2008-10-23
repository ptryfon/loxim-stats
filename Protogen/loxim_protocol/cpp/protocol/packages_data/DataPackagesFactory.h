#ifndef DATAPACKAGESFACTORY_H_
#define DATAPACKAGESFACTORY_H_

#include <stdint.h>
#include "../ptools/Package.h"

namespace protocol{

class DataPackagesFactory{
public:
	static Package *createPackageNotInit(uint8_t type);
	static Package *createPackage(uint8_t type, char* data);
	static Package *createPackageContent(uint8_t type,PackageBufferReader* reader);
};}
#endif /*DATAPACKAGESFACTORY_H_*/

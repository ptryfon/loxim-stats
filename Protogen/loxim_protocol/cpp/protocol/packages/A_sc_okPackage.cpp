#include <string.h>
#include <stdlib.h>
#include "A_sc_okPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

A_sc_okPackage::A_sc_okPackage()
{
}

A_sc_okPackage::~A_sc_okPackage()
{
}

void A_sc_okPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
}

bool A_sc_okPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	return true;
}

bool A_sc_okPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	A_sc_okPackage* p=(A_sc_okPackage*)_package;
	return true;
}

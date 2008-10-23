#include <string.h>
#include <stdlib.h>
#include "V_sc_finishedPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

V_sc_finishedPackage::V_sc_finishedPackage()
{
}

V_sc_finishedPackage::~V_sc_finishedPackage()
{
}

void V_sc_finishedPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
}

bool V_sc_finishedPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	return true;
}

bool V_sc_finishedPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	V_sc_finishedPackage* p=(V_sc_finishedPackage*)_package;
	return true;
}

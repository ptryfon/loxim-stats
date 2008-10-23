#include <string.h>
#include <stdlib.h>
#include "W_c_modePackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

W_c_modePackage::W_c_modePackage(\
		enum ModesEnum a_new_mode
		)
{
	new_mode=a_new_mode;
}

W_c_modePackage::W_c_modePackage()
{
}

W_c_modePackage::~W_c_modePackage()
{
}

void W_c_modePackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	uint64_t tmp_new_mode;
	ModesFactory::getValueByEnum(new_mode, tmp_new_mode);
	writer->writeUint64(tmp_new_mode);
}

bool W_c_modePackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	ModesFactory::getEnumByValue(reader->readUint64(),new_mode);
	return true;
}

bool W_c_modePackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	W_c_modePackage* p=(W_c_modePackage*)_package;
	if (!(getNew_mode()==p->getNew_mode())) return false;
	return true;
}

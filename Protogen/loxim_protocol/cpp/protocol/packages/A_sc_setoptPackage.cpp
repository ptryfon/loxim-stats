#include <string.h>
#include <stdlib.h>
#include "A_sc_setoptPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

A_sc_setoptPackage::A_sc_setoptPackage(\
		CharArray* a_key,\
		CharArray* a_value
		)
{
	key=a_key;
	value=a_value;
}

A_sc_setoptPackage::A_sc_setoptPackage()
{
	key=NULL;
	value=NULL;
}

A_sc_setoptPackage::~A_sc_setoptPackage()
{
	if (key) delete key;
	if (value) delete value;
}

void A_sc_setoptPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeSstring(key);
	writer->writeString(value);
}

bool A_sc_setoptPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	key=reader->readSstring();
	value=reader->readString();
	return true;
}

bool A_sc_setoptPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	A_sc_setoptPackage* p=(A_sc_setoptPackage*)_package;
	if (!(CharArray::equal(getKey(),p->getKey()))) return false;
	if (!(CharArray::equal(getValue(),p->getValue()))) return false;
	return true;
}

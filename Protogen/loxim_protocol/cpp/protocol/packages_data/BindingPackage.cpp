#include <string.h>
#include <stdlib.h>
#include "BindingPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

BindingPackage::BindingPackage(\
		CharArray* a_bindingName,\
		int64_t a_type,\
		Package* a_value
		)
{
	bindingName=a_bindingName;
	type=a_type;
	value=a_value;
}

BindingPackage::BindingPackage()
{
	bindingName=NULL;
	type=0;
	value=NULL;
}

BindingPackage::~BindingPackage()
{
	if (bindingName) delete bindingName;
	if(value) delete value;
	value=NULL;
}

void BindingPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeSstring(bindingName);
	writer->writeVaruint(type);
	value->serializeContent(writer);
}

bool BindingPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	bindingName=reader->readSstring();
	{bool b; type=reader->readVaruint(b);}
	value=DataPackagesFactory::createPackageContent(type,reader);
	return true;
}

bool BindingPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	BindingPackage* p=(BindingPackage*)_package;
	if (!(CharArray::equal(getBindingName(),p->getBindingName()))) return false;
	if (!(getType()==p->getType())) return false;
	if (!((!getValue() && ! p->getValue())||(getValue() && p->getValue() && (getValue()->equals(p->getValue()))))) return false;
	return true;
}

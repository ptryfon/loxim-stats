/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/Data/StructPackage.h>

using namespace std;

namespace Protocol {
	StructPackage::StructPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream):

		CollectionPackage(mask, cancel, length, stream)
	{
	}

	StructPackage::StructPackage()
	{
	}

	uint8_t StructPackage::get_type() const
	{
		return STRUCT_PACKAGE;
	}

}
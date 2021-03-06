/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/Data/VoidPackage.h>

using namespace std;

namespace Protocol {
	VoidPackage::VoidPackage(const sigset_t &, const bool &, size_t &, DataStream &)

	{
	}

	VoidPackage::VoidPackage()
	{
	}

	void VoidPackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
	}

	uint8_t VoidPackage::get_type() const
	{
		return VOID_PACKAGE;
	}

	string VoidPackage::to_string() const
	{
		stringstream ss;
		ss << "VoidPackage:" << endl;
		return ss.str();
	}

	size_t VoidPackage::get_ser_size() const
	{
		return 0;
	}

}

/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/Data/Uint64Package.h>

using namespace std;

namespace Protocol {
	Uint64Package::Uint64Package(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream):

		value(stream.read_uint64(mask, cancel, length))
	{
	}

	Uint64Package::Uint64Package(uint64_t value):

		value(value)
	{
	}

	void Uint64Package::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
		stream.write_uint64(mask, cancel, value);
	}

	uint8_t Uint64Package::get_type() const
	{
		return UINT64_PACKAGE;
	}

	string Uint64Package::to_string() const
	{
		stringstream ss;
		ss << "Uint64Package:" << endl;
		ss << "  value: " << value << endl;
		return ss.str();
	}

	size_t Uint64Package::get_ser_size() const
	{
		return 0 + 8;
	}

	uint64_t Uint64Package::get_val_value() const
	{
		return value;
	}
}

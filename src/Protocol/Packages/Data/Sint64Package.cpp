/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/Data/Sint64Package.h>

using namespace std;

namespace Protocol {
	Sint64Package::Sint64Package(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream):


		value(stream.read_int64(mask, cancel, length))
	{
	}

	Sint64Package::Sint64Package(int64_t value):

		value(value)
	{
	}

	void Sint64Package::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
		stream.write_int64(mask, cancel, value);
	}

	uint8_t Sint64Package::get_type() const
	{
		return SINT64_PACKAGE;
	}

	string Sint64Package::to_string() const
	{
		stringstream ss;
		ss << "Sint64Package:" << endl;
		ss << "  value: " << value << endl;
		return ss.str();
	}

	size_t Sint64Package::get_ser_size() const
	{
		return 0 + 8;
	}

	int64_t Sint64Package::get_val_value() const
	{
		return value;
	}
}
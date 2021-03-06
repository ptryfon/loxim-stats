/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/Data/DoublePackage.h>

using namespace std;

namespace Protocol {
	DoublePackage::DoublePackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream):

		value(stream.read_double(mask, cancel, length))
	{
	}

	DoublePackage::DoublePackage(double value):

		value(value)
	{
	}

	void DoublePackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
		stream.write_double(mask, cancel, value);
	}

	uint8_t DoublePackage::get_type() const
	{
		return DOUBLE_PACKAGE;
	}

	string DoublePackage::to_string() const
	{
		stringstream ss;
		ss << "DoublePackage:" << endl;
		ss << "  value: " << value << endl;
		return ss.str();
	}

	size_t DoublePackage::get_ser_size() const
	{
		return 0 + 8;
	}

	double DoublePackage::get_val_value() const
	{
		return value;
	}
}

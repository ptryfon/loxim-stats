/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/Data/BindingPackage.h>

using namespace std;

namespace Protocol {
	BindingPackage::BindingPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream):


		binding_name(stream.read_string(mask, cancel, length)),
		value(DataPackageFactory::deserialize_unknown(mask, cancel, length, stream))
	{
	}

	BindingPackage::BindingPackage(std::auto_ptr<ByteBuffer> binding_name, std::auto_ptr<Package> value):

		binding_name(binding_name),
		value(value)
	{
	}

	void BindingPackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
		stream.write_string(mask, cancel, binding_name);
		stream.write_varuint(mask, cancel, VarUint(value->get_type(), false));
		value->serialize(mask, cancel, stream, false);
	}

	uint8_t BindingPackage::get_type() const
	{
		return BINDING_PACKAGE;
	}

	string BindingPackage::to_string() const
	{
		stringstream ss;
		ss << "BindingPackage:" << endl;
		ss << "  binding_name: " << binding_name->get_const_data() << endl;
		ss << "  value: " << value->to_string() << endl;
		return ss.str();
	}

	size_t BindingPackage::get_ser_size() const
	{
		return 0 + binding_name->get_ser_size() + value->get_ser_size() + DataStream::get_varuint_size(VarUint(value->get_type(), false));
	}

	const ByteBuffer &BindingPackage::get_val_binding_name() const
	{
		return *binding_name.get();
	}
	const Package &BindingPackage::get_val_value() const
	{
		return *value.get();
	}
}
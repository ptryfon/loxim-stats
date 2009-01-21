#include <string>
#include <sstream>
#include <Protocol/Exceptions.h>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/Data/CollectionPackage.h>

using namespace std;
using namespace _smptr;

namespace Protocol {
	CollectionPackage::CollectionPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream)
	{
		VarUint v_count = stream.read_varuint(mask, cancel, length);
		if (v_count.is_null())
			throw ProtocolLogic();
		uint64_t count = v_count.get_val();
		VarUint global_type = stream.read_varuint(mask, cancel, length);
		same_type = !global_type.is_null();
		for (uint64_t i = 0; i < count; ++i){
			if (same_type){
				if (global_type.get_val() > 255)
					throw ProtocolLogic();
				auto_ptr<Package> pkg = DataPackageFactory::deserialize(mask, cancel, (uint8_t)global_type.get_val(), length, stream);
				packages.push_back(shared_ptr<Package>(pkg));
			} else {
				auto_ptr<Package> pkg = DataPackageFactory::deserialize_unknown(mask, cancel, length, stream);
				packages.push_back(shared_ptr<Package>(pkg));
			}
		}
	}

	CollectionPackage::CollectionPackage() : same_type(true)
	{
	}

	void CollectionPackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
		stream.write_varuint(mask, cancel, VarUint(packages.size(), false));
		if (same_type && (packages.size() > 0))
			stream.write_varuint(mask, cancel, VarUint(packages[0]->get_type(), false));
		else{
			//the protocol doesn't specify, what should be
			//the type of an empty collection
			stream.write_varuint(mask, cancel, VarUint(0, true));
		}
		for (vector<shared_ptr<Package> >::const_iterator i = packages.begin(); i != packages.end(); ++i){
			if (!same_type)
				stream.write_varuint(mask, cancel, VarUint((*i)->get_type(), false));
			(*i)->serialize(mask, cancel, stream, false);
		}
	}

	string CollectionPackage::to_string() const
	{
		stringstream ss;
		ss << "CollectionPackage:" << endl;
		for (vector<shared_ptr<Package> >::const_iterator i = packages.begin(); i != packages.end(); ++i){
			ss << (*i)->to_string() << endl;
		}
		return ss.str();
	}

	size_t CollectionPackage::get_ser_size() const
	{
		size_t res = DataStream::get_varuint_size(VarUint(packages.size(), false));
		if (same_type && packages.size())
			res += DataStream::get_varuint_size(VarUint(packages[0]->get_type(), false));
		else{
			res += DataStream::get_varuint_size(VarUint(0, true));
		}
		for (vector<shared_ptr<Package> >::const_iterator i = packages.begin(); i != packages.end(); ++i){
			if (!same_type)
				res += DataStream::get_varuint_size(VarUint((*i)->get_type(), false));
			res += (*i)->get_ser_size();
		}
		return res;
	}

	const vector<shared_ptr<Package> > &CollectionPackage::get_packages() const
	{
		return packages;
	}

	void CollectionPackage::push_back(std::auto_ptr<Package> package)
	{
		packages.push_back(shared_ptr<Package>(package));
		if (same_type && (packages.size() > 1)){
			size_t size = packages.size();
			same_type = packages[size-1] == packages[size-2];
		}
	}

}


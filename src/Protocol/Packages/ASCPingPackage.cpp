/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/ASCPingPackage.h>

using namespace std;

namespace Protocol {
	ASCPingPackage::ASCPingPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream)

	{
	}

	ASCPingPackage::ASCPingPackage()
	{
	}

	void ASCPingPackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
	}

	uint8_t ASCPingPackage::get_type() const
	{
		return A_SC_PING_PACKAGE;
	}

	string ASCPingPackage::to_string() const
	{
		stringstream ss;
		ss << "ASCPingPackage:" << endl;
		return ss.str();
	}

	size_t ASCPingPackage::get_ser_size() const
	{
		return 0;
	}

}
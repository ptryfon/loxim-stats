/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/ASCPongPackage.h>

using namespace std;

namespace Protocol {
	ASCPongPackage::ASCPongPackage(const sigset_t &, const bool &, size_t &, DataStream &)

	{
	}

	ASCPongPackage::ASCPongPackage()
	{
	}

	void ASCPongPackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
	}

	uint8_t ASCPongPackage::get_type() const
	{
		return A_SC_PONG_PACKAGE;
	}

	string ASCPongPackage::to_string() const
	{
		stringstream ss;
		ss << "ASCPongPackage:" << endl;
		return ss.str();
	}

	size_t ASCPongPackage::get_ser_size() const
	{
		return 0;
	}

}

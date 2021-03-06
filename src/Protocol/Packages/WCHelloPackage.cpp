/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/WCHelloPackage.h>

using namespace std;

namespace Protocol {
	WCHelloPackage::WCHelloPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream):

		pid(stream.read_int64(mask, cancel, length)),
		client_name(stream.read_string(mask, cancel, length)),
		client_version(stream.read_string(mask, cancel, length)),
		hostname(stream.read_string(mask, cancel, length)),
		language(stream.read_string(mask, cancel, length)),
		collation(stream.read_uint64(mask, cancel, length)),
		timezone(stream.read_int8(mask, cancel, length))
	{
	}

	WCHelloPackage::WCHelloPackage(int64_t pid, std::auto_ptr<ByteBuffer> client_name, std::auto_ptr<ByteBuffer> client_version, std::auto_ptr<ByteBuffer> hostname, std::auto_ptr<ByteBuffer> language, uint64_t collation, int8_t timezone):

		pid(pid),
		client_name(client_name),
		client_version(client_version),
		hostname(hostname),
		language(language),
		collation(collation),
		timezone(timezone)
	{
	}

	void WCHelloPackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
		stream.write_int64(mask, cancel, pid);
		stream.write_string(mask, cancel, client_name);
		stream.write_string(mask, cancel, client_version);
		stream.write_string(mask, cancel, hostname);
		stream.write_string(mask, cancel, language);
		stream.write_uint64(mask, cancel, collation);
		stream.write_int8(mask, cancel, timezone);
	}

	uint8_t WCHelloPackage::get_type() const
	{
		return W_C_HELLO_PACKAGE;
	}

	string WCHelloPackage::to_string() const
	{
		stringstream ss;
		ss << "WCHelloPackage:" << endl;
		ss << "  pid: " << pid << endl;
		ss << "  client_name: " << client_name->get_const_data() << endl;
		ss << "  client_version: " << client_version->get_const_data() << endl;
		ss << "  hostname: " << hostname->get_const_data() << endl;
		ss << "  language: " << language->get_const_data() << endl;
		ss << "  collation: " << collation << endl;
		ss << "  timezone: " << ((int)timezone) << endl;
		return ss.str();
	}

	size_t WCHelloPackage::get_ser_size() const
	{
		return 0 + 8 + client_name->get_ser_size() + client_version->get_ser_size() + hostname->get_ser_size() + language->get_ser_size() + 8 + 1;
	}

	int64_t WCHelloPackage::get_val_pid() const
	{
		return pid;
	}
	const ByteBuffer &WCHelloPackage::get_val_client_name() const
	{
		return *client_name.get();
	}
	const ByteBuffer &WCHelloPackage::get_val_client_version() const
	{
		return *client_version.get();
	}
	const ByteBuffer &WCHelloPackage::get_val_hostname() const
	{
		return *hostname.get();
	}
	const ByteBuffer &WCHelloPackage::get_val_language() const
	{
		return *language.get();
	}
	uint64_t WCHelloPackage::get_val_collation() const
	{
		return collation;
	}
	int8_t WCHelloPackage::get_val_timezone() const
	{
		return timezone;
	}
}

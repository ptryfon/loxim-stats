/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#include <Protocol/Streams/PackageCodec.h>
#include <Protocol/Packages/Package.h>
#include <Protocol/Exceptions.h>
#include <Protocol/PackageFactory.h>

using namespace std;

namespace Protocol {
	PackageCodec::PackageCodec(auto_ptr<DataStream> stream, size_t limit) :
		stream(stream), limit(limit)
	{
	}

	auto_ptr<Package> PackageCodec::read_package(const sigset_t &mask, const
			bool &cancel)
	{
		stream->prefetch(mask, cancel, 5);
		size_t limit = this->limit;
		uint8_t type = stream->read_uint8(mask, cancel, limit);
		size_t size = stream->read_uint32(mask, cancel, limit);
		if (size > limit)
			throw PackageTooBig();
		stream->prefetch(mask, cancel, size);
		return PackageFactory::deserialize(mask,
				cancel, type, size, *stream);
	}

	auto_ptr<Package> PackageCodec::read_package_expect(const sigset_t
			&mask, const bool &cancel, uint8_t type)
	{
		auto_ptr<Package> pkg = read_package(mask, cancel);
		if (pkg->get_type() != type)
			throw ProtocolLogic();
		return pkg;
	}

	void PackageCodec::write_package(const sigset_t &mask, const bool
			&cancel, const Package& p)
	{
		p.serialize(mask, cancel, *stream);
		stream->flush(mask, cancel);
	}
}


#ifndef PACKAGE_CODEC_H
#define PACKAGE_CODEC_H

#include <signal.h>
#include <memory>
#include <stdint.h>
#include <Protocol/Streams/PackageStream.h>


namespace Protocol {
	class Package;
	class DataStream;
	class PackageCodec : public PackageStream {
		private:
			DataStream &stream;
			const size_t limit;
		public:
			PackageCodec(DataStream &, size_t limit);
			virtual std::auto_ptr<Package> read_package(const
					sigset_t&, const bool &);
			virtual	std::auto_ptr<Package> read_package_excpect(
					const sigset_t&, const bool &, uint8_t);
			virtual void write_package(const sigset_t&, const bool&,
					const Package&);
	};
}

#endif /* PACKAGE_CODEC_H */

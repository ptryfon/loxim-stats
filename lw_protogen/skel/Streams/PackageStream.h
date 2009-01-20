#ifndef PACKAGE_STREAM_H
#define PACKAGE_STREAM_H


#include <signal.h>
#include <memory>

namespace Protocol {
	class Package;
	
	class PackageStream{
		public:
			virtual std::auto_ptr<Package> read_package(
					const sigset_t&, const bool &) = 0;
			virtual	std::auto_ptr<Package> read_package_excpect(
					const sigset_t &, const bool &,
					uint8_t) = 0;
			virtual void write_package(const sigset_t&, const bool&,
					const Package&) = 0;
			virtual ~PackageStream() = 0;

	};
}
#endif /* PACKAGE_STREAM_H */



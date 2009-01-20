#ifndef PROTOCOL_PACKAGE_H
#define PROTOCOL_PACKAGE_H

#include <signal.h>
#include <string>
#include <Protocol/Streams/DataStream.h>

namespace Protocol {
	class Package {
		public:
			virtual void serialize(const sigset_t &mask, const bool&
					cancel, DataStream &stream, bool
					with_header = true) const = 0;
			virtual uint8_t get_type() const = 0;
			virtual ~Package();
			virtual std::string to_string() const = 0;
			/* how many bytes will serialization take? */
			virtual size_t get_ser_size() const = 0;
	};
}

#endif /* PROTOCOL_PACKAGE_H */

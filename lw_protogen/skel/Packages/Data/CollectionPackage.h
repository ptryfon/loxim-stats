#ifndef PROTOCOL_COLLECTION_PACKAGE_H
#define PROTOCOL_COLLECTION_PACKAGE_H

#include <string>
#include <memory>
#include <vector>
#include <Util/smartptr.h>
#include <Protocol/Packages/Package.h>
#include <Protocol/ByteBuffer.h>

namespace Protocol {
	class CollectionPackage : public Package {
		private:
			std::vector<_smptr::shared_ptr<Package> > packages;
			bool same_type;
		public:
			CollectionPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream);
			CollectionPackage();

			void serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header = true) const;
			uint8_t get_type() const = 0;
			std::string to_string() const;
			size_t get_ser_size() const;
			const std::vector<_smptr::shared_ptr<Package> > &get_packages() const;
			void push_back(std::auto_ptr<Package>);
	};
}
#endif

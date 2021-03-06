#ifndef PROTOCOL_EXTERNAL_REF_PACKAGE_H
#define PROTOCOL_EXTERNAL_REF_PACKAGE_H

/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <memory>
#include <Protocol/Packages/Package.h>
#include <Protocol/ByteBuffer.h>

namespace Protocol {
	class ExternalRefPackage : public Package {
		private:
			uint64_t value_id;
			uint64_t stamp;
		public:
			ExternalRefPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream);
			ExternalRefPackage(uint64_t value_id, uint64_t stamp);

			void serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header = true) const;
			uint8_t get_type() const;
			std::string to_string() const;
			size_t get_ser_size() const;
			uint64_t get_val_value_id() const;
			uint64_t get_val_stamp() const;
	};
}
#endif

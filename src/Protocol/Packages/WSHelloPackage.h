#ifndef PROTOCOL_W_S_HELLO_PACKAGE_H
#define PROTOCOL_W_S_HELLO_PACKAGE_H

/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <memory>
#include <Protocol/Packages/Package.h>
#include <Protocol/ByteBuffer.h>

namespace Protocol {
	class WSHelloPackage : public Package {
		private:
			uint8_t protocol_major;
			uint8_t protocol_minor;
			uint8_t system_major;
			uint8_t system_minor;
			uint32_t max_package_size;
			uint64_t features;
			uint64_t auth_methods;
			std::auto_ptr<ByteBuffer> salt;
		public:
			WSHelloPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream);
			WSHelloPackage(uint8_t protocol_major, uint8_t protocol_minor, uint8_t system_major, uint8_t system_minor, uint32_t max_package_size, uint64_t features, uint64_t auth_methods, std::auto_ptr<ByteBuffer> salt);

			void serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header = true) const;
			uint8_t get_type() const;
			std::string to_string() const;
			size_t get_ser_size() const;
			uint8_t get_val_protocol_major() const;
			uint8_t get_val_protocol_minor() const;
			uint8_t get_val_system_major() const;
			uint8_t get_val_system_minor() const;
			uint32_t get_val_max_package_size() const;
			uint64_t get_val_features() const;
			uint64_t get_val_auth_methods() const;
			const ByteBuffer &get_val_salt() const;
	};
}
#endif

#ifndef PROTOCOL_TIMETZ_PACKAGE_H
#define PROTOCOL_TIMETZ_PACKAGE_H

/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <memory>
#include <Protocol/Packages/Package.h>
#include <Protocol/ByteBuffer.h>

namespace Protocol {
	class TimetzPackage : public Package {
		private:
			uint8_t hour;
			uint8_t minutes;
			uint8_t sec;
			int16_t milis;
			int8_t tz;
		public:
			TimetzPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream);
			TimetzPackage(uint8_t hour, uint8_t minutes, uint8_t sec, int16_t milis, int8_t tz);

			void serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header = true) const;
			uint8_t get_type() const;
			std::string to_string() const;
			size_t get_ser_size() const;
			uint8_t get_val_hour() const;
			uint8_t get_val_minutes() const;
			uint8_t get_val_sec() const;
			int16_t get_val_milis() const;
			int8_t get_val_tz() const;
	};
}
#endif

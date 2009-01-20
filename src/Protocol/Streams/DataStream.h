/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#ifndef DATA_STREAM_H
#define DATA_STREAM_H

#include <memory>
#include <signal.h>
#include <stdint.h>
#include <Protocol/VarUint.h>


namespace Protocol {
	
	class ByteBuffer;

	class DataStream {
		public:
			static size_t get_varuint_size(VarUint v);

			virtual ~DataStream() = 0;
			virtual void read(char *, size_t len, const sigset_t &,
					const bool &cancel) = 0;
			virtual void write(const char *, size_t len, const
					sigset_t &, const bool &cancel) = 0;
			virtual uint8_t read_uint8(const sigset_t &, const bool
					&cancel, size_t &limit) = 0;
			virtual int8_t read_int8(const sigset_t &, const bool
					&cancel, size_t &limit) = 0;
			virtual uint16_t read_uint16(const sigset_t &, const
					bool &cancel, size_t &limit) = 0;
			virtual int16_t read_int16(const sigset_t &, const bool
					&cancel, size_t &limit) = 0;
			virtual uint32_t read_uint32(const sigset_t &, const
					bool &cancel, size_t &limit) = 0;
			virtual int32_t read_int32(const sigset_t &, const bool
					&cancel, size_t &limit) = 0;
			virtual uint64_t read_uint64(const sigset_t &, const
					bool &cancel, size_t &limit) = 0;
			virtual int64_t read_int64(const sigset_t &, const bool
					&cancel, size_t &limit) = 0;
			virtual double read_double(const sigset_t &, const bool
					&cancel, size_t &limit) = 0;
			virtual VarUint read_varuint(const sigset_t &, const
					bool &cancel, size_t &limit) = 0;
			virtual bool read_bool(const sigset_t &, const
					bool &cancel, size_t &limit) = 0;
			virtual std::auto_ptr<ByteBuffer> read_bytes(const
					sigset_t &, const bool &cancel, size_t
					&limit) = 0;
			virtual std::auto_ptr<ByteBuffer> read_n_bytes(const
					sigset_t &, const bool &cancel, size_t n, 
					size_t &limit) = 0;
			virtual std::auto_ptr<ByteBuffer> read_string(const
					sigset_t &, const bool &cancel, size_t
					&limit) = 0;
			virtual size_t write_uint8(const sigset_t &, const bool
					&cancel, uint8_t data) = 0;
			virtual size_t write_int8(const sigset_t &, const bool
					&cancel, int8_t data) = 0;
			virtual size_t write_uint16(const sigset_t &, const bool
					&cancel, uint16_t data) = 0;
			virtual size_t write_int16(const sigset_t &, const bool
					&cancel, int16_t data) = 0;
			virtual size_t write_uint32(const sigset_t &, const bool
					&cancel, uint32_t data) = 0;
			virtual size_t write_int32(const sigset_t &, const bool
					&cancel, int32_t data) = 0;
			virtual size_t write_uint64(const sigset_t &, const bool
					&cancel, uint64_t data) = 0;
			virtual size_t write_int64(const sigset_t &, const bool
					&cancel, int64_t data) = 0;
			virtual size_t write_double(const sigset_t &, const bool
					&cancel, double data) = 0;
			virtual size_t write_varuint(const sigset_t &, const bool
					&cancel, VarUint data) = 0;
			virtual size_t write_bool(const sigset_t &, const bool
					&cancel, bool data) = 0;
			virtual size_t write_bytes(const sigset_t &, const bool
					&cancel, const std::auto_ptr<ByteBuffer>
					&data) = 0;
			virtual size_t write_string(const sigset_t &, const bool
					&cancel, const std::auto_ptr<ByteBuffer>
					&data) = 0;
			virtual void prefetch(const sigset_t &, const bool &,
					const size_t size) = 0;
			virtual void flush(const sigset_t &, const bool &) = 0;
	};
}
#endif /* DATA_STREAM_H */



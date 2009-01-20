/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#ifndef FILE_DATA_STREAM_H
#define FILE_DATA_STREAM_H


#include <signal.h>
#include <Util/smartptr.h>
#include <Protocol/Streams/DataStream.h>

namespace Protocol {
	
	class FileDataStream : public DataStream {
		protected:
			int fd;
			bool big_endian;
			static inline uint64_t swap64(uint64_t x) {
				return x<<56 |
					(x & (uint64_t)0x000000000000ff00ULL)<<40 |
					(x & (uint64_t)0x0000000000ff0000ULL)<<24 |
					(x & (uint64_t)0x00000000ff000000ULL)<< 8 |
					(x & (uint64_t)0x000000ff00000000ULL)>> 8 |
					(x & (uint64_t)0x0000ff0000000000ULL)>>24 |
					(x & (uint64_t)0x00ff000000000000ULL)>>40 |
					x>>56;
			}
		public:
			/* the second argument determines wether to convert from
		 * networnorder */
			FileDataStream(int fd); 
			virtual void read(char *, size_t len, const sigset_t &,
					const bool &cancel);
			virtual void write(const char *, size_t len, const
					sigset_t &, const bool &cancel);
			virtual uint8_t read_uint8(const sigset_t &, const bool
					&cancel, size_t &limit);
			virtual int8_t read_int8(const sigset_t &, const bool
					&cancel, size_t &limit);
			virtual uint16_t read_uint16(const sigset_t &, const
					bool &cancel, size_t &limit);
			virtual int16_t read_int16(const sigset_t &, const bool
					&cancel, size_t &limit);
			virtual uint32_t read_uint32(const sigset_t &, const
					bool &cancel, size_t &limit);
			virtual int32_t read_int32(const sigset_t &, const bool
					&cancel, size_t &limit);
			virtual uint64_t read_uint64(const sigset_t &, const
					bool &cancel, size_t &limit);
			virtual int64_t read_int64(const sigset_t &, const bool
					&cancel, size_t &limit);
			virtual double read_double(const sigset_t &, const bool
					&cancel, size_t &limit);
			virtual VarUint read_varuint(const sigset_t &, const
					bool &cancel, size_t &limit);
			virtual bool read_bool(const sigset_t &, const
					bool &cancel, size_t &limit);
			virtual std::auto_ptr<ByteBuffer> read_bytes(const
					sigset_t &, const bool &cancel, size_t
					&limit);
			virtual std::auto_ptr<ByteBuffer> read_n_bytes(const
					sigset_t &, const bool &cancel, size_t n, 
					size_t &limit);
			/* 
			 * Current implementation calls read_bytes, but this is
			 * wrong: it should be UTF aware and swap bytes on
			 * multi-byte characters
			 */
			virtual std::auto_ptr<ByteBuffer> read_string(const
					sigset_t &, const bool &cancel, size_t
					&limit);
			virtual size_t write_uint8(const sigset_t &, const bool
					&cancel, uint8_t data);
			virtual size_t write_int8(const sigset_t &, const bool
					&cancel, int8_t data);
			virtual size_t write_uint16(const sigset_t &, const bool
					&cancel, uint16_t data);
			virtual size_t write_int16(const sigset_t &, const bool
					&cancel, int16_t data);
			virtual size_t write_uint32(const sigset_t &, const bool
					&cancel, uint32_t data);
			virtual size_t write_int32(const sigset_t &, const bool
					&cancel, int32_t data);
			virtual size_t write_uint64(const sigset_t &, const bool
					&cancel, uint64_t data);
			virtual size_t write_int64(const sigset_t &, const bool
					&cancel, int64_t data);
			virtual size_t write_double(const sigset_t &, const bool
					&cancel, double data);
			virtual size_t write_varuint(const sigset_t &, const bool
					&cancel, VarUint data);
			virtual size_t write_bool(const sigset_t &, const bool
					&cancel, bool data);
			virtual size_t write_bytes(const sigset_t &, const bool
					&cancel, const std::auto_ptr<ByteBuffer>
					&data);
			/* 
			 * Current implementation calls write_bytes, but this is
			 * wrong: it should be UTF aware and swap bytes on
			 * multi-byte characters
			 */
			virtual size_t write_string(const sigset_t &, const bool
					&cancel, const std::auto_ptr<ByteBuffer>
					&data);
			virtual void prefetch(const sigset_t &, const bool &,
					const size_t size);
			virtual void flush(const sigset_t &, const bool &);
			~FileDataStream();
	};
}
#endif /* FILEDATA_STREAM_H */



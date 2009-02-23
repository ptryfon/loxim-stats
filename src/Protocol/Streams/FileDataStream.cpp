/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#include <sys/select.h>
#include <netinet/in.h>
#include <cassert>
#include <Protocol/Streams/FileDataStream.h>
#include <Protocol/Exceptions.h>
#include <Protocol/ByteBuffer.h>
#include <Util/Concurrency.h>


using namespace std;
using namespace Util;

namespace Protocol {

	FileDataStream::FileDataStream(int fd) : fd(fd)
	{
		big_endian = ((htons(1) == 1));
	}

	//either reads or throws
	void FileDataStream::read(char *buf, size_t len, const sigset_t &mask,
			const bool &cancel)
	{
		while (len){
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			int res;
			{
				Masker m(mask);
				if (cancel)
					throw OperationCancelled();
				res = pselect(fd+1, &fds, NULL, NULL, NULL,
						&m.get_old_mask());
			}
			if (res < 0){
				if (errno == EINTR && cancel)
					throw OperationCancelled();
				throw ReadError(errno);
			}
			if (res != 1){
				//the OS is cheating ;)
				throw ReadError(EIO);
			}
			res = ::read(fd, buf, len);
			if (res < 0){
				if (errno == EINTR && cancel)
					throw OperationCancelled();
				throw ReadError(errno);
			}
			if (res == 0)
				throw ReadError(ENODATA);
			len -= res;
		}
	}

	void FileDataStream::write(const char *buf, size_t len, const sigset_t
			&mask, const bool &cancel)
	{
		int offset = 0;
		while (len){
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			int res;
			{
				Masker m(mask);
				if (cancel)
					throw OperationCancelled();
				res = pselect(fd+1, NULL, &fds, NULL, NULL,
						&m.get_old_mask());
			}
			if (res < 0){
				if (errno == EINTR && cancel)
					throw OperationCancelled();
				throw WriteError(errno);
			}
			if (res != 1){
				//the OS is cheating ;)
				throw WriteError(EIO);
			}
			res = ::write(fd, buf + offset, len);
			if (res < 0){
				if (errno == EINTR && cancel)
					throw OperationCancelled();
				throw WriteError(errno);
			}
			if (res == 0)
				throw WriteError(ENODATA);
			offset += res;
			len -= res;
		}
	}

	FileDataStream::~FileDataStream()
	{
		close(fd);
	}
			
	uint8_t FileDataStream::read_uint8(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 1)
			throw ProtocolLogic();
		uint8_t res;
		read((char*)&res, sizeof(res), mask, cancel);
		limit -= 1;
		return res;
	}

	int8_t FileDataStream::read_int8(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 1)
			throw ProtocolLogic();
		int8_t res;
		read((char*)&res, sizeof(res), mask, cancel);
		limit -= 1;
		return res;
	}

	uint16_t FileDataStream::read_uint16(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 2)
			throw ProtocolLogic();
		uint16_t res;
		read((char*)&res, sizeof(res), mask, cancel);
		limit -= 2;
		return ntohs(res);
	}

	int16_t FileDataStream::read_int16(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 2)
			throw ProtocolLogic();
		int16_t res;
		read((char*)&res, sizeof(res), mask, cancel);
		limit -= 2;
		return ntohs(res);
	}

	uint32_t FileDataStream::read_uint32(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 4)
			throw ProtocolLogic();
		uint32_t res;
		limit -= 4;
		read((char*)&res, sizeof(res), mask, cancel);
		return ntohl(res);
	}

	int32_t FileDataStream::read_int32(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 4)
			throw ProtocolLogic();
		uint32_t res;
		read((char*)&res, sizeof(res), mask, cancel);
		limit -= 4;
		return ntohl(res);
	}

	uint64_t FileDataStream::read_uint64(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 8)
			throw ProtocolLogic();
		uint64_t res;
		read((char*)&res, sizeof(res), mask, cancel);
		limit -= 8;
		if (!big_endian)
			return swap64(res);
		return res;
	}

	int64_t FileDataStream::read_int64(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		if (limit < 8)
			throw ProtocolLogic();
		uint64_t res;
		read((char*)&res, sizeof(res), mask, cancel);
		limit -= 8;
		if (!big_endian)
			return swap64(res);
		return res;
	}
			
	double FileDataStream::read_double(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		uint64_t res = read_uint64(mask, cancel, limit);
		return *((double*)&res);

	}
	
	VarUint FileDataStream::read_varuint(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		uint8_t fb = read_uint8(mask, cancel, limit);
		if (fb < 250)
			return VarUint(fb, false);
		switch (fb){
			case 250: return VarUint(0, true);
			case 251: return VarUint(read_uint16(mask, cancel, limit), false); 
			case 252: return VarUint(read_uint32(mask, cancel, limit), false);
			case 253: return VarUint(read_uint64(mask, cancel, limit), false);
			default: throw ProtocolLogic();
		}
	}

	bool FileDataStream::read_bool(const sigset_t &mask, const bool
			&cancel, size_t &limit)
	{
		return read_uint8(mask, cancel, limit);
	}

	auto_ptr<ByteBuffer> FileDataStream::read_bytes(const sigset_t &mask,
			const bool &cancel, size_t &limit)
	{
		VarUint len = read_varuint(mask, cancel, limit);
		if (len.is_null())
			return auto_ptr<ByteBuffer>(new ByteBuffer(0));
		if (len.get_val() > limit)
			throw ProtocolLogic();
		return read_n_bytes(mask, cancel, (size_t)len.get_val(), limit);
	}
			
	std::auto_ptr<ByteBuffer> FileDataStream::read_n_bytes(const sigset_t
			&mask, const bool &cancel, size_t n, size_t &limit)
	{
		auto_ptr<ByteBuffer> buf(new ByteBuffer(n));
		read(buf->get_data(), n, mask, cancel);
		limit -= n;
		return buf;
	}
	
	auto_ptr<ByteBuffer> FileDataStream::read_string(const sigset_t &mask,
			const bool &cancel, size_t &limit)
	{
		return read_bytes(mask, cancel, limit);
	}

	size_t FileDataStream::write_uint8(const sigset_t &mask, const bool
			&cancel, uint8_t data)
	{
		write((char*)&data, sizeof(data), mask, cancel);
		return 1;
	}

	size_t FileDataStream::write_int8(const sigset_t &mask, const bool
			&cancel, int8_t data)
	{
		write((char*)&data, sizeof(data), mask, cancel);
		return 1;
	}

	size_t FileDataStream::write_uint16(const sigset_t &mask, const bool
			&cancel, uint16_t data)
	{
		uint16_t hlp = htons(data);
		write((char*)&hlp, sizeof(hlp), mask, cancel);
		return 2;
	}

	size_t FileDataStream::write_int16(const sigset_t &mask, const bool
			&cancel, int16_t data)
	{
		int16_t hlp = htons(data);
		write((char*)&hlp, sizeof(hlp), mask, cancel);
		return 2;
	}

	size_t FileDataStream::write_uint32(const sigset_t &mask, const bool
			&cancel, uint32_t data)
	{
		uint32_t hlp = htonl(data);
		write((char*)&hlp, sizeof(hlp), mask, cancel);
		return 4;
	}

	size_t FileDataStream::write_int32(const sigset_t &mask, const bool
			&cancel, int32_t data)
	{
		int32_t hlp = htonl(data);
		write((char*)&hlp, sizeof(hlp), mask, cancel);
		return 4;
	}

	size_t FileDataStream::write_uint64(const sigset_t &mask, const bool
			&cancel, uint64_t data)
	{
		uint64_t hlp;
		if (!big_endian)
			hlp = swap64(data);
		else
			hlp = data;
		write((char*)&hlp, sizeof(hlp), mask, cancel);
		return 8;
	}

	size_t FileDataStream::write_int64(const sigset_t &mask, const bool
			&cancel, int64_t data)
	{
		int64_t hlp;
		if (!big_endian)
			hlp = swap64(data);
		else
			hlp = data;
		write((char*)&hlp, sizeof(hlp), mask, cancel);
		return 8;
	}
			
	size_t FileDataStream::write_double(const sigset_t &mask, const bool
			&cancel, double data)
	{
		assert(sizeof(double) == 8);
		return write_uint64(mask, cancel, *((uint64_t*)&data));
	}

	size_t FileDataStream::write_varuint(const sigset_t &mask, const bool
			&cancel, VarUint data)
	{
		if (data.is_null()){
			write_uint8(mask, cancel, 250);
			return 1;
		}
		if (data.get_val() < 250){
			write_uint8(mask, cancel, (uint8_t)data.get_val());
			return 1;
		}
		if (data.get_val() <= 0xFFFFU){
			write_uint8(mask, cancel, 251);
			write_uint16(mask, cancel, (uint16_t)data.get_val());
			return 3;
		}
		if (data.get_val() <= 0xFFFFFFFFUL){
			write_uint8(mask, cancel, 252);
			write_uint32(mask, cancel, (uint32_t)data.get_val());
			return 5;
		}
		write_uint8(mask, cancel, 253);
		write_uint64(mask, cancel, data.get_val());
		return 9;

	}

	size_t FileDataStream::write_bool(const sigset_t &mask, const bool
			&cancel, bool data)
	{
		uint8_t hlp = data?1:0;
		return write_uint8(mask, cancel, hlp);
	}

	size_t FileDataStream::write_bytes(const sigset_t &mask, const bool
			&cancel, const auto_ptr<ByteBuffer> &data)
	{
		size_t len = write_varuint(mask, cancel, VarUint(data->get_size(), false));
		write(data->get_const_data(), data->get_size(), mask, cancel);
		return len + data->get_size();
	}

	/* 
	 * Current implementation calls write_bytes, but this is
	 * wrong: it should be UTF aware and swap bytes on
	 * multi-byte characters
	 */
	size_t FileDataStream::write_string(const sigset_t &mask, const bool
			&cancel, const auto_ptr<ByteBuffer> &data)
	{
		return write_bytes(mask, cancel, data);
	}

	void FileDataStream::prefetch(const sigset_t &, const bool &, const
			size_t size)
	{
	}

	void FileDataStream::flush(const sigset_t &, const bool &)
	{
	}
}



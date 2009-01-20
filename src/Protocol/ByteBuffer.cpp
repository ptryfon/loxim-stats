/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#include <string>
#include <memory>
#include <Protocol/ByteBuffer.h>
#include <Protocol/Streams/DataStream.h>

namespace Protocol {


	ByteBuffer::ByteBuffer(size_t size) : size(size), buffer(new char[size])
	{
	}

	ByteBuffer::ByteBuffer(const char *data, size_t size) : size(size),
	buffer(new char[size])
	{
		memcpy(buffer, data, size);
	}

	ByteBuffer::ByteBuffer(const std::string &s) : size(s.length()), buffer(new
			char[s.length()])
	{
		memcpy(buffer, s.c_str(), size);
	}
	
	ByteBuffer::ByteBuffer(const ByteBuffer& buf2) : size(buf2.size),
	buffer(new char[buf2.size])
	{
		memcpy(buffer, buf2.buffer, size);
	}
	
	ByteBuffer::~ByteBuffer()
	{
		delete[] buffer;
	}

	ByteBuffer &ByteBuffer::operator=(const ByteBuffer& buf2)
	{
		size = buf2.size;
		char *tmp_buf = new char[size];
		delete[] buffer;
		buffer = tmp_buf;
		memcpy(buffer, buf2.buffer, size);
		return *this;
	}

	char *ByteBuffer::get_data()
	{
		return buffer;
	}

	const char *ByteBuffer::get_const_data() const
	{
		return buffer;
	}

	size_t ByteBuffer::get_size() const
	{
		return size;
	}

	size_t ByteBuffer::get_ser_size() const
	{
		return DataStream::get_varuint_size(VarUint(size, false)) + size;
	}

}

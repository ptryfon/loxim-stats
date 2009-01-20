#include <string>
#include <memory>
#include <Protocol/ByteBuffer.h>
#include <Protocol/Streams/DataStream.h>

namespace Protocol {

	//only for testing, will be removed
	ByteBuffer::ByteBuffer()
	{
	}

	ByteBuffer::ByteBuffer(size_t size) : size(size), buffer(new char[size])
	{
	}

	ByteBuffer::ByteBuffer(const char *data, size_t size) : size(size),
	buffer(new char[size])
	{
		memcpy(buffer.get(), data, size);
	}

	ByteBuffer::ByteBuffer(const std::string &s) : size(s.length()), buffer(new
			char[s.length()])
	{
		memcpy(buffer.get(), s.c_str(), size);
	}
	
	ByteBuffer::ByteBuffer(const ByteBuffer& buf2) : size(buf2.size),
	buffer(new char[buf2.size])
	{
		memcpy(buffer.get(), buf2.buffer.get(), size);
	}

	ByteBuffer &ByteBuffer::operator=(const ByteBuffer& buf2)
	{
		size = buf2.size;
		buffer = std::auto_ptr<char>(new char[size]);
		memcpy(buffer.get(), buf2.buffer.get(), size);
		return *this;
	}

	char *ByteBuffer::get_data()
	{
		return buffer.get();
	}

	const char *ByteBuffer::get_const_data() const
	{
		return buffer.get();
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

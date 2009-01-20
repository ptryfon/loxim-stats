#ifndef PROTOCOL_BYTE_BUFFER_H
#define PROTOCOL_BYTE_BUFFER_H

#include <memory>
#include <string.h>

namespace Protocol{
	class ByteBuffer{
		private:
			int size;
			std::auto_ptr<char> buffer;
		public:
			ByteBuffer(size_t size);
			ByteBuffer(const char *data, size_t size);
			ByteBuffer(const std::string &s);
			ByteBuffer(const ByteBuffer& buf2);
			ByteBuffer &operator=(const ByteBuffer& buf2);
			char *get_data();
			const char *get_const_data() const;
			size_t get_size() const;
			/* how many bytes will the serialization take */
			size_t get_ser_size() const;

			//XXX Only for development - will be removed SOON
			ByteBuffer();

	};
}

#endif /* PROTOCOL_BYTE_BUFFER_H */

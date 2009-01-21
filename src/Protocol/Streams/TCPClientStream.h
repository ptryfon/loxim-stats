/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#ifndef PROTOCOL_TCP_CLIENT_STREAM_H
#define PROTOCOL_TCP_CLIENT_STREAM_H

#include <memory>
#include <Protocol/Streams/FileDataStream.h>

namespace Protocol {
	class TCPClientStream : public FileDataStream {
		private:
			int connect(uint32_t address, uint16_t port);
		public:
			/* 
			 * Creates a TCP/IP client binding to address address
			 * and port port. address and port should be in network
			 * order.
			 */
			TCPClientStream(uint32_t address, uint16_t port);

	};
}

#endif /* PROTOCOL_TCP_CLIENT_STREAM_H */

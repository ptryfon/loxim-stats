/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#ifndef PROTOCOL_TCP_SERVER_H
#define PROTOCOL_TCP_SERVER_H

#include <memory>
#include <Protocol/Streams/DataStream.h>

namespace Protocol {
	class TCPServer {
		private:
			int sock;
			void set_non_block(bool);
			bool should_repeat(int);
		public:
			/* 
			 * Creates a TCP/IP server binding to address address
			 * and port port. The address may be INADDR_ANY. For the
			 * meaning of backlog, please refer to listen(2).
			 */
			TCPServer(uint32_t address, uint16_t port, int backlog);
			std::auto_ptr<DataStream> accept(const sigset_t &, const bool
					&cancel);
	};
}

#endif /* PROTOCOL_TCP_SERVER_H */

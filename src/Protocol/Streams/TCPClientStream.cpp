/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <Protocol/Exceptions.h>
#include <Protocol/Streams/TCPClientStream.h>
#include <Protocol/Streams/FileDataStream.h>
#include <Util/Masker.h>


using namespace std;
using namespace Util;

namespace Protocol {
		
	TCPClientStream::TCPClientStream(uint32_t address, uint16_t port) :
		FileDataStream(connect(address, port))
	{
	}

	int TCPClientStream::connect(uint32_t address, uint16_t port)
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0)
			throw ConnectionError(errno);
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = port;
		sa.sin_addr.s_addr = address;

		if (::connect(sock, (struct sockaddr*)&sa, sizeof(sa)))
			throw ConnectionError(errno);
		return sock;
	}

}


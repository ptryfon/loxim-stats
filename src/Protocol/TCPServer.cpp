/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <Protocol/TCPServer.h>
#include <Protocol/Exceptions.h>
#include <Protocol/Streams/FileDataStream.h>
#include <Util/Concurrency.h>


using namespace std;
using namespace Util;

namespace Protocol {
		
	TCPServer::TCPServer(uint32_t address, uint16_t port, int backlog)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0)
			throw ConnectionError(errno);
		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = port;
		sa.sin_addr.s_addr = address;
		/* allows binding even if TCP/IP stack is still in TIMED_WAIT state
		 * if it doesn't work, ignore it. It is not that important */
		u_int yes=1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

		if (bind(sock, (struct sockaddr*)&sa, sizeof(sa)))
			throw ConnectionError(errno);
		if (listen(sock, backlog) < 0)
			throw ConnectionError(errno);
	}

	/* 
	 * We use it to be certain, that accept won't block, because otherwise it
	 * would not be possible to use cancel and signals to cancel the accept
	 * due to races.
	 */
	void TCPServer::set_non_block(bool val)
	{
		if (val)
			fcntl(F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
		else
			fcntl(F_SETFL, fcntl(sock, F_GETFL) & ~O_NONBLOCK);

	}

	bool TCPServer::should_repeat(int error)
	{
		return error == ENETDOWN || error ==  EPROTO || error == ENOPROTOOPT ||
			error == EHOSTDOWN || error == EHOSTUNREACH ||
			error == EOPNOTSUPP || error == ENETUNREACH || error == EAGAIN ||
			error == EWOULDBLOCK;
	}


	auto_ptr<DataStream> TCPServer::accept(const sigset_t &mask, const bool &cancel)
	{
		while (!cancel){
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(sock, &fds);
			set_non_block(false);
			int res;
			{
				Masker m(mask);
				if (cancel)
					throw OperationCancelled();
				res = pselect(sock+1, &fds, NULL, NULL, NULL,
						&m.get_old_mask());
			}
			if (res < 0){
				if (errno == EINTR && cancel)
					throw OperationCancelled();
				throw ConnectionError(errno);
			}
			if (res != 1)
				//the OS is cheating ;)
				throw ConnectionError(EIO);
			set_non_block(true);
			res = ::accept(sock, NULL, NULL);
			if (res < 0){
				if (errno == EINTR && cancel)
					throw OperationCancelled();
				if (should_repeat(errno))
					continue;
				throw ConnectionError(errno);
			}
			return auto_ptr<DataStream>(new FileDataStream(res));
		}
		throw OperationCancelled();
	}
}


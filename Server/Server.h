#ifndef SERVER_H_
#define SERVER_H_

#include <winsock2.h>

class Server
{
public:
	Server();
	virtual ~Server();
	int createSocket(int port);
	int listenOnSock(SOCKET sock);
	int run();
private:
	int createThread(SOCKET sock);
};

#endif /*SERVER_H_*/

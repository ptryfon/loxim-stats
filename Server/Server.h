#ifndef SERVER_H_
#define SERVER_H_
#include <sys/socket.h>

//namespace Server {
	class Server
	{
	public:
		Server(int sock);
		virtual ~Server();
		
		int Run();
	// Made public for testing
		int Send(char* buf, int buf_size);

		int initializeAll();
		
	private:
		int Sock;
		char messgBuff[60];

	//TCP methods
		int Receive(int* receiveDataSize);
		int Disconnect();
	};
//}

#endif /*SERVER_H_*/

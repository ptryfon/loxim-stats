#ifndef SERVER_H_
#define SERVER_H_
#include <sys/socket.h>

#define MESSGBUF_SIZE 128

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
		char messgBuff[MESSGBUF_SIZE];

	//TCP methods
		int Receive(int* receiveDataSize);
		int Disconnect();
	};
//}

#endif /*SERVER_H_*/

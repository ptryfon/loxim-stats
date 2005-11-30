#ifndef SERVER_H_
#define SERVER_H_
#include <sys/socket.h>

#define MAX_MESSG 4096
#define MAX_MESSG_HEADER_LEN 1
#define SERIALIZE_DEPTH 1

#include "../QueryExecutor/QueryResult.h"
using namespace QExecutor;

//namespace Server {
	class Server
	{
	public:
		Server(int sock);
		virtual ~Server();
		
		int Run();
	// Made public for testing
		int Send(char* buf, int buf_size);
		int Serialize(QueryResult *qr, char **buffer, char **bufferStart);

		int InitializeAll();
		
	private:
		int Sock;

	//TCP methods
		int Receive(char** buffer, int* receiveDataSize);
		int Disconnect();
	};
//}

#endif /*SERVER_H_*/

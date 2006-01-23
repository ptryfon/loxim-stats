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
		
		//DEVELOPING - NOT YET USED
		int SerializeRec(QueryResult *qr);		
		double htonDouble(double in);
		//int BExit();
	private:
		char *bigBuf;
		int Sock;
		char *serialBuf;
		char *serialBufBegin;
		char *serialBufEnd;
		int serialBufSize;
		
		int sendError(int errNo);
		//void sigHandler(int sig);
	//TCP methods
		int Receive(char** buffer, int* receiveDataSize);
		int Disconnect();
	};
//}

#endif /*SERVER_H_*/

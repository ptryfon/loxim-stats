#ifndef SERVER_H_
#define SERVER_H_
#include <sys/socket.h>
#include <pthread.h>

#define MAX_MESSG 4096
#define MAX_MESSG_HEADER_LEN 1
#define SERIALIZE_DEPTH 1

#include "../QueryExecutor/QueryExecutor.h"
#include "../QueryParser/QueryParser.h"
#include "../QueryExecutor/QueryResult.h"
using namespace QExecutor;

extern pthread_t pthread_master_id;
extern void sigHandler(int arg);

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
		//static void sigHandler(int sig);
			
		int SExit(int code);
		int SerializeRec(QueryResult *qr);		
		double htonDouble(double in);
	private:
		char *bigBuf;
		int Sock;
		char *messgBuff;
		char *serialBuf;
		char *serialBufBegin;
		char *serialBufEnd;
		int serialBufSize;
		ErrorConsole *ec;

		int sendError(int errNo);
		QueryExecutor *qEx;
		QueryParser *qPa;
		
	//TCP methods
		int Receive(char** buffer, int* receiveDataSize);
		int Disconnect();
	};
//}

#endif /*SERVER_H_*/

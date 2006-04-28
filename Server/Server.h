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

extern int getPTindex(pthread_t p);
extern int emptyPocket();
extern pthread_mutex_t mut;
extern pthread_t pthread_master_id;
extern void sigHandler(int arg);
extern int max_thread_index;
extern int running_threads_count;
extern int pulseThread_index;
extern pthread_t pthreads[];
extern pthread_t pulseThreads[];

//namespace Server {
	enum SERVER_OPERATION_STATUS {
	    SRV_DEFAULT,
	    SRV_PROCESSING_QUERY,
	    SRV_INITIALIZING
	};

	class Server
	{
	public:
		Server(int sock);
		virtual ~Server();
		
		int Run();
		int Send(char* buf, int buf_size);
		int Serialize(QueryResult *qr, char **buffer, char **bufferStart);
		int Receive(char** buffer, int* receiveDataSize);
		void Test();
		void cancelPC(pthread_t p);
		
		QueryExecutor* getQEx();		
		int getSocket();
		int getFdContact();
		int pulseCheckerNotify();
		//static void sigHandler(int sig);
			
		SERVER_OPERATION_STATUS srvStatus;
		
		int SExit(int code);
		int SerializeRec(QueryResult *qr);		
		double htonDouble(double in);
	private:
		char *bigBuf;
		int Sock;
		int fdContact;
		char *messgBuff;
		char *serialBuf;
		char *serialBufBegin;
		char *serialBufEnd;
		int serialBufSize;
		ErrorConsole *ec;
		pthread_t pulseChecker_id;

		int sendError(int errNo);
		QueryExecutor *qEx;
		QueryParser *qPa;
		
		int Disconnect();
	};
//}

#endif /*SERVER_H_*/

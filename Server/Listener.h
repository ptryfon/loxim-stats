#ifndef LISTENER_H_
#define LISTENER_H_
#define MAX_CLIENTS 16
#include <signal.h>
#include "../Errors/ErrorConsole.h"
#include "../Config/SBQLConfig.h"

using namespace Errors;
using namespace Config;

//namespace Listener 
//{
	typedef enum LISTENER_STATUS
	{
		ERROR = -1,
		SUCCESS 
	};

/*	
 * A.D.
 *  Waits(listens) for client to communicate.
 *  Creates Server object for each connection	 
 */
        struct doubleSock {
	    int sock;
	    int pcSock;
	};

	class Listener
	{
	public:
		Listener();
		virtual ~Listener();	
		
	// Run Listener - start listening and do stuff
		int Start(int port);
	// Made public for testing	
		int CreateSocket(int port, int* created_socket);
		static int CreateSocketStatic(int port, int* created_socket);
		int Lock(void);
		int Unlock(void);
	#define limit 5	// default max number of incomming connections at the same time
	
		static int ListenOnSocket(int sock, int* newSocket, int queueLength=limit);
		static int CloseSocket(int sock);
		
	private:
		ErrorConsole* lCons;
		
	// TCP methods
		
		
	
	// Creates Server class object for given connection
		int CreateServer(int sock);
	//Server routine for Posix threads
	//	void *createServ(void *arg);
	
		//pthread_t pthreads[MAX_CLIENTS];
		int thread_sockets[MAX_CLIENTS];
		doubleSock thread_sockets_2[MAX_CLIENTS];
		//int threads_count;
		
	};
	
	extern Listener *ls;
//}
#endif /*LISTENER_H_*/

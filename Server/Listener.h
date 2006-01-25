#ifndef LISTENER_H_
#define LISTENER_H_
#define MAX_CLIENTS 16
#include <signal.h>

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

	class Listener
	{
	public:
		Listener();
		virtual ~Listener();	
		
	// Run Listener - start listening and do stuff
		int Start(int port);
	// Made public for testing	
		int CreateSocket(int port, int* created_socket);
				
	private:
	// TCP methods
		int ListenOnSocket(int sock, int* newSocket);
		int CloseSocket(int sock);
		
	
	// Creates Server class object for given connection
		int CreateServer(int sock);
	//Server routine for Posix threads
	//	void *createServ(void *arg);
	
		//pthread_t pthreads[MAX_CLIENTS];
		int thread_sockets[MAX_CLIENTS];
		//int threads_count;
		
	};
//}
#endif /*LISTENER_H_*/

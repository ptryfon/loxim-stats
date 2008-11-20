#ifndef TCPIPCLIENTSOCKET_H_
#define TCPIPCLIENTSOCKET_H_

#include <protocol/sockets/AbstractSocket.h>

#define SOCKET_CONNECTION_STATUS_OK     1
#define SOCKET_CONNECTION_STATUS_CLOSED -1
#define SOCKET_CONNECTION_STATUS_UNKNOWN_HOST -2
#define SOCKET_CONNECTION_STATUS_OPENING_STREAM -3
#define SOCKET_CONNECTION_STATUS_CANNOT_CONNECT -4

namespace protocol{

class TCPIPClientSocket: public AbstractSocket
{
	protected:
		char*	host;
		int	port;
		int	sock;
		
		AbstractInputStream* inputStream;
		AbstractOutputStream* outputStream;
			
	public:	
		TCPIPClientSocket(char* host,int port);
		~TCPIPClientSocket();
				
		virtual AbstractInputStream  *getInputStream();	
		virtual AbstractOutputStream *getOutputStream();
		
		/**
		 *  zwraca jedn± ze sta³ych: SOCKET_CONNECTION_
		 * 
		 * generalnie >0 - OK
		 * <0            - B³±d
		 */		
		int connect();
		void close();
		
	private:
		
};
}
#endif /*TCPIPCLIENTSOCKET_H_*/

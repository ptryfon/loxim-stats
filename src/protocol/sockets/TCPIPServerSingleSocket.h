#ifndef TCPIPSERVERSINGLESOCKET_H_
#define TCPIPSERVERSINGLESOCKET_H_

#include "AbstractSocket.h"

#define SOCKET_CONNECTION_STATUS_OK     1
#define SOCKET_CONNECTION_STATUS_CLOSED -1
#define SOCKET_CONNECTION_STATUS_UNKNOWN_HOST -2
#define SOCKET_CONNECTION_STATUS_OPENING_STREAM -3
#define SOCKET_CONNECTION_STATUS_CANNOT_CONNECT -4

namespace protocol{

class TCPIPServerSingleSocket: public AbstractSocket
{
	protected:
		int	sock;
		
		AbstractInputStream* inputStream;
		AbstractOutputStream* outputStream;
			
	public:	
		TCPIPServerSingleSocket(int a_socket);
		~TCPIPServerSingleSocket();
				
		virtual AbstractInputStream  *getInputStream();	
		virtual AbstractOutputStream *getOutputStream();
		
		void close();		
	private:
		
};
}
#endif /*TCPIPCLIENTSOCKET_H_*/

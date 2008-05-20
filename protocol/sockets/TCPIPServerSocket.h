#ifndef TCPIPSERVERSOCKET_H_
#define TCPIPSERVERSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>

#include "TCPIPClientSocket.h"
//#define SOCKET_CONNECTION_STATUS_OK     1
//#define SOCKET_CONNECTION_STATUS_CLOSED -1
//#define SOCKET_CONNECTION_STATUS_UNKNOWN_HOST -2
//#define SOCKET_CONNECTION_STATUS_OPENING_STREAM -3
//#define SOCKET_CONNECTION_STATUS_CANNOT_CONNECT -4

namespace protocol{

class TCPIPServerSocket
{
	protected:
		char*	host;
		int	port;
		int	sock;	
		
	public:
		/**
	 	* Je¶li <code>host==null</code> to serwer s³ucha na wszystkich adresach
	 	* komputera lokalnego - w przeciwnym przypadku - na wybranym adresie.
	 	*/	
		TCPIPServerSocket(char* host,int port);	
		virtual ~TCPIPServerSocket(){};
		
		int bind();					
		AbstractSocket *accept();
		AbstractSocket *accept(long timeout);
		int getAcceptError();
		void close();
		
	private:
		/**
		 * Zgodnie z linuksowym manulem, po czesci bledow z accept, nalezy je
		 * poworzyc. To sprawdza czy dla danego kodu nalezy
		 */
		bool shouldRepeat(int error);
		int acceptError;
		void setNonBlock(bool val);

};
}
#endif /*TCPIPSERVERSOCKET_H_*/

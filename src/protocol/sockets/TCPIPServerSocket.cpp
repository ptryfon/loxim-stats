#include <protocol/sockets/TCPIPServerSocket.h>
#include <protocol/sockets/TCPIPServerSingleSocket.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET_BIND_ERROR -2
#define SOCKET_UNKNOWN_BIND_HOST -3
#define SOCKET_BIND_STATUS_OPENING_STREAM -4


#define  QUEUE_LENGTH 100

using namespace protocol;

TCPIPServerSocket::TCPIPServerSocket(char* a_host,int a_port)
{
	host=a_host?strdup(a_host):NULL;
	port=a_port;
	sock=-1;
}

void TCPIPServerSocket::close()
{	
	::close(sock);
	if(host)
	{
		free(host);
		host=NULL;
	};
}
		
int TCPIPServerSocket::bind()
{	
	struct sockaddr_in server;
	struct hostent *hp;
	int res;	
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		return SOCKET_BIND_STATUS_OPENING_STREAM;
	}
	
	server.sin_family = AF_INET;
	if (host!=NULL)
	{
		/* What is the Internet address (xxx.xxx.xxx.xxx) of this machine? */
		hp = gethostbyname (host);
		if (hp == NULL)
		{
			return SOCKET_UNKNOWN_BIND_HOST;
		}	
		memcpy ((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
	}else{
		server.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	/* Functions htonx() transform from host to network byte order. */
	server.sin_port = htons (port);
	
	/* allows binding even if TCP/IP stack is still in TIMED_WAIT state 
	 * if it doesn't work, ignore it. It is not that important */
	u_int yes=1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	/* Associate the address with the socket. */
  	if ((res = ::bind (sock, (struct sockaddr *) &server, sizeof server) < 0))
    {
	 perror("bind returned %d\n");
    	 return SOCKET_BIND_ERROR;
    }
    
    if (::listen (sock, QUEUE_LENGTH) < 0)
    {
      perror ("listening stream socket");
      exit (EXIT_FAILURE);
    }
	return SOCKET_CONNECTION_STATUS_OK;
}


bool TCPIPServerSocket::shouldRepeat(int error)
{
	return error == ENETDOWN || error ==  EPROTO || error == ENOPROTOOPT ||
	     error == EHOSTDOWN || error ==  ENONET || error == EHOSTUNREACH ||
	     error == EOPNOTSUPP || error == ENETUNREACH;
}


AbstractSocket *TCPIPServerSocket::accept()
{
	int msgsoc=0;
	setNonBlock(false);
	do
	{				
		msgsoc=::accept(sock,NULL,NULL);
		
	}while(msgsoc < 0 && shouldRepeat(errno));
	if (msgsoc<0){
		acceptError = errno;
		return NULL;
	}
	acceptError = 0;
	return new TCPIPServerSingleSocket(msgsoc);			
}


AbstractSocket *TCPIPServerSocket::accept(sigset_t *sigmask, int *cancel)
{
	sigset_t old_mask;
	while (!*cancel){
		fd_set rdfd;
		FD_ZERO(&rdfd);
		FD_SET(sock, &rdfd);
		setNonBlock(false);
		pthread_sigmask(SIG_SETMASK, sigmask, &old_mask);
		if (*cancel){
			acceptError = ECANCELED;
			pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
			return NULL;
		}
		int res = pselect(sock + 1, &rdfd, NULL, NULL, NULL, &old_mask);
		pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
		if (res > 0){
			setNonBlock(true);			
			res = ::accept(sock, NULL, NULL);
			if (res == -1){
				if (errno != EAGAIN && errno != EWOULDBLOCK && !shouldRepeat(errno)){
					acceptError = errno;
					return NULL;
				} else
					continue;	
			} else {
				return new TCPIPServerSingleSocket(res);
			}
		}else{
			if (res < 0){
				if (errno == EINTR)
					acceptError = ECANCELED;
				else
					acceptError = errno;
			} else
				acceptError = ETIMEDOUT;
			return NULL;	
		}
	}
}


int TCPIPServerSocket::getAcceptError()
{
	return acceptError;
}

void TCPIPServerSocket::setNonBlock(bool val)
{
	if (val)
	    fcntl(F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
	else
	    fcntl(F_SETFL, fcntl(sock, F_GETFL) & ~O_NONBLOCK);
}

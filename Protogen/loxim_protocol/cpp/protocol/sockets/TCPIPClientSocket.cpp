#include "TCPIPClientSocket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../streams/DescriptorInputStream.h"
#include "../streams/DescriptorOutputStream.h"

using namespace protocol;

TCPIPClientSocket::TCPIPClientSocket(char* a_host,int a_port)
{
	inputStream=NULL;
	outputStream=NULL;
	host=strdup(a_host);
	port=a_port;
	sock=-1;
}

TCPIPClientSocket::~TCPIPClientSocket()
{
	if (inputStream!=NULL)
	{
		inputStream->close();
		delete inputStream;
		inputStream=NULL;
	}
		
	if (outputStream!=NULL)
	{
		outputStream->close();
		delete outputStream;
		outputStream=NULL;
	}
	
	free(host);
}		

void TCPIPClientSocket::close()
{	
	if (inputStream!=NULL)
	{
		inputStream->close();
		delete inputStream;
		inputStream=NULL;
	}
		
	if (outputStream!=NULL)
	{
		outputStream->close();
		delete outputStream;
		outputStream=NULL;
	}
		
	::close(sock);
}
		
int TCPIPClientSocket::connect()
{	
	struct sockaddr_in server;
	struct hostent *hp;
		
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		return SOCKET_CONNECTION_STATUS_OPENING_STREAM;
	}
	
	/* What is the Internet address (xxx.xxx.xxx.xxx) of this machine? */
	hp = gethostbyname (host);
	if (hp == NULL)
	{
		return SOCKET_CONNECTION_STATUS_UNKNOWN_HOST;
	}
	
	server.sin_family = AF_INET;
	memcpy ((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
	/* Functions htonx() transform from host to network byte order. */
	server.sin_port = htons (port);
	
	if (::connect (sock, (struct sockaddr *) &server, sizeof server) < 0)
	{
		return SOCKET_CONNECTION_STATUS_CANNOT_CONNECT;
	};
	return SOCKET_CONNECTION_STATUS_OK;
}

  
AbstractInputStream* TCPIPClientSocket::getInputStream()
{
	if ((inputStream==NULL)&&(sock>=0))
	{
		inputStream=new DescriptorInputStream(sock);
	} 
	return inputStream;
}
	
AbstractOutputStream* TCPIPClientSocket::getOutputStream()
{
	if ((outputStream==NULL)&&(sock>=0))
	{
		outputStream=new DescriptorOutputStream(sock);
	}
	return outputStream;
} 

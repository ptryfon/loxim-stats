#include <protocol/sockets/TCPIPServerSingleSocket.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <protocol/streams/DescriptorInputStream.h>
#include <protocol/streams/DescriptorOutputStream.h>

using namespace protocol;

TCPIPServerSingleSocket::TCPIPServerSingleSocket(int a_socket)
{
	sock=a_socket;
	inputStream=NULL;
	outputStream=NULL;
}

void TCPIPServerSingleSocket::close()
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

TCPIPServerSingleSocket::~TCPIPServerSingleSocket()
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
}
		
  
AbstractInputStream* TCPIPServerSingleSocket::getInputStream()
{
	if ((inputStream==NULL)&&(sock>=0))
	{
		inputStream=new DescriptorInputStream(sock);
	} 
	return inputStream;
}
	
AbstractOutputStream* TCPIPServerSingleSocket::getOutputStream()
{
	if ((outputStream==NULL)&&(sock>=0))
	{
		outputStream=new DescriptorOutputStream(sock);
	}
	return outputStream;
} 

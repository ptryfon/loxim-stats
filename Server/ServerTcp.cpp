#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

#include "Server.h"
#include "../TCPProto/Tcp.h"

using namespace std;
using namespace TCPProto;

/* klasa obslugujaca konkretne polaczenie musi zawierac te dwa pola
 *

private:
	char buffer[60];
	int socket; //tutaj w konstruktorze trzeba przypisac gniazdo

*/


int Server::Receive(char** buffer, int* receiveDataSize) { 
    return bufferReceive(buffer, receiveDataSize, Sock);
    return 0;
}

int Server::Send(char* buf, int buf_size) {
	return bufferSend(buf, buf_size, Sock);
return 0;
}

int Server::Disconnect(){
	printf("ServerTcp: disconnecting \n");
	return close(Sock);
}


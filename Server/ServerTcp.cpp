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
//	*ec << "ServerTcp: disconnecting";
	int res = close(Sock);
	if (res != 0) {
		ec->printf("blad zamykania gniazda watku, gzniado: %d, blad: %s\n", Sock, strerror(errno));
		return res | ErrServer;
	} else {
	//	ec->printf("<ServerTcp::disconnect> gniazdo watku: %d zamknieto pomyslnie\n", Sock);
	return 0;
	}
}



#include <stdio.h>
#include <stdlib.h>
#include "Listener.h"
#include "Server.h"

//using namespace Listener;

Listener::Listener()
{
}

Listener::~Listener()
{
}

int Listener::CreateServer(int sock) {
	Server *srv = new Server(sock); 
	printf("Listener: running new server thread \n");
	return srv->Run();
}


int Listener::Start(int port) {
	
	int sock;
	int newSock;
	
	CreateSocket(port, &sock);
	ListenOnSocket(sock, &newSock);
	CreateServer(newSock);
	//CloseSocket(sock);
	
	return 0;

}


int main(int argc, char* argv[]) {
	//return 0;
	int port = 6543;
	if (argc>1) 
		port = atoi(argv[1]);
	printf("Listener: running port ----> %d\n", port);
	/*
	char buffer[4096];
	char dupa[3]="AB";
	char pupa[3]="CD";
	char *bufPoint;
	
	memset(buffer, '\0', 4096);
	bufPoint = buffer;
	memcpy(bufPoint, dupa, 3);
	printf("buffer is %s\n", buffer);
	printf("bufPoint is %s\n", bufPoint);
	bufPoint++;
	bufPoint++;
	printf("buffer is %s \n", buffer);
	printf("buffPoint is %s \n", bufPoint);
	memcpy(bufPoint, pupa, 3);
	printf("buffer is %s \n bufPoint is %s \n", buffer, bufPoint);
	return 0;
	*/
	Listener *ls = new Listener();
	ls->Start(port);
	printf("Listener: ends \n");
	return 0;	
}




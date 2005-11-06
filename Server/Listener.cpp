
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
	ListenOnSocket(sock, &sock);
	CreateServer(sock);
	//CloseSocket(sock);
	
	return 0;

}


int main(int argc, char* argv[]) {
	int port = 6543;
	if (argc>1) 
		port = atoi(argv[1]);
	printf("Listener: running port ----> %d\n", port);
	Listener *ls = new Listener();
	ls->Start(port);
	printf("Listener: ends \n");
	return 0;	
}




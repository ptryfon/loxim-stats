//Test file - but don't delete it please!

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include "Listener.h"
#include "Server.h"

int main (int argc, char *argv[]) {
	
	int sock;
	int port = 6543;
	char buf[1];
	int len = 1;
	
	buf[0] = 'K';
	
	if (argc > 1) 
		port = atoi(argv[1]);
	
	Listener *Ls = new Listener();
	printf("Tu!\n");
	Ls->CreateSocket(port, &sock);
	printf("Tu!\n");
		
	Server *Srv = new Server(sock);
	Srv->Send(buf, len);		
		
	return 0;
}




 
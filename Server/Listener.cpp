
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
	return CreateServer(newSock);
	//CloseSocket(sock);
	
	//return 0;

}


//char *bufB;
//char *buf;
//int counter;

//TESTING
/* 
int recWrite() {
    char mapa[4]="DUP";
    char mapa2[2]="2";
    char **bufL=&buf;
    char type='X';

    printf("%c \n", bufB[0]);
    buf[0]=type;
    *bufL++;
    printf("Inside..");
    memcpy(*bufL, mapa, 4);
    *bufL=*bufL+3;
    printf("buf: |%s|, bufL: |%s|, bufB: |%s| \n", buf,* bufL, bufB);
    memcpy(*bufL, mapa2, 2);
    *bufL=*bufL+1;
    printf("buf: |%s|, bufL: |%s|, bufB: |%s| \n", buf,* bufL, bufB);
    counter++;    
    printf("counter %d\n", counter);
    if (counter<3)
	recWrite();
    return 0;
}
*/
int main(int argc, char* argv[]) {
	//return 0;
	int port = 6543;
	if (argc>1) 
		port = atoi(argv[1]);
	printf("Listener: running port ----> %d\n", port);
/*
	int size=4096;
	bufB =(char *)malloc(size);
	char dupa[3]="AB";
	char pupa[3]="CD";
	buf=bufB;
	char *bufE=bufB+size;
	counter=0;	
	
	memcpy(buf, dupa, 3);
	printf("buffer is %s\n", buf);
	printf("bufB is %s\n", bufB);
	buf++;
	buf++;
	printf("buffer is %s \n", buf);
	printf("bufB is %s \n", bufB);
	memcpy(buf, pupa, 3);
	printf("buffer is %s \n bufB is %s \n", buf, bufB);
	buf++;
	buf++;
	printf("Entering recWrite");
	recWrite();
	printf("buf is %s \n bufB is %s \n", buf, bufB);
	return 0;
*/
	int res;
	Listener *ls = new Listener();
	if ((res=(ls->Start(port)))!=0) {
	    printf("Listener: ends with ERROR: %d \n", res);
	    return res;
	}
	printf("Listener: ends \n");
	return 0;	
}




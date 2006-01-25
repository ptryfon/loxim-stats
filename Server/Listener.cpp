
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Listener.h"
#include "Server.h"

//using namespace Listener;

pthread_t pthreads[MAX_CLIENTS];
int threads_count;

Listener::Listener()
{
    memset(thread_sockets, '\0', MAX_CLIENTS);
    threads_count=0;
}

Listener::~Listener()
{
}

int Listener::CreateServer(int sock) {
	Server *srv = new Server(sock); 
	printf("Listener: running new server thread \n");
	return srv->Run();
}

void *exitPoint() {
    pthread_exit((void *) 0);
}

void sigHandler(int sig) {
    unsigned int pself;
    int i;
    int errorCode;
    int status;
    
    pself=pthread_self();
    printf("Listener-sigHandler: my pthread_id is %d and ", pself);
    if (pself==pthread_master_id) {
	printf(".. i a the Listener thread \n");
	for (i=0;i<threads_count;i++) {
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			printf("Listener-sigHandler-Master: ERROR - failed to join thread: %s\n", strerror(errorCode));
			exit(errorCode);
		    }   
		    if (status!=0) {
			printf("Listener-sigHandler-Master: Server thread nr. |%d| terminated with error\n", i);
			exit(1);
		    }
		    else {
			printf("Listener-sigHandler-Master: JOINED thread nr.|%d| with pthread_id |%d|\n", i, pthreads[i]);
		    } 
	}
	printf("Listener-sigHandler-Master: All threads joined - SUCCESS! Quitting now.. \n");
	exit(0); 
    }
    else {
	printf(".. I am a Server thread\n");
	printf("Listener-sigHandler-Thread%d: Exiting \n", pself); 
	exitPoint();
    }	
}    
    
void *createServ(void *arg) {
    int socket=*(int *)arg;
    int res;
    printf("Listener-createServ: New server thread reporting. My socket number is |%d|\n", socket);
    if (socket<0) {
	("Listener-createServ: .. which is really unfortunate. I must quit now\n");
	pthread_exit((void *)socket);
    }	
    Server *srv = new Server(socket);
    printf("Listener-createServ: Executing server code now.. \n");
    res=srv->Run();
    printf("Listener-createServ: Server returned |%d|, exiting \n", res); 
    pthread_exit((void *)res);
}

int Listener::Start(int port) {
	
	int sock;
	int newSock=-1;
	int isEnd=0;
	int errorCode=0;
	int i=0;
	int status;

	sigset_t lBlock_cc;
	sigemptyset(&lBlock_cc);
	sigaddset(&lBlock_cc, SIGINT);	
	
	pthread_master_id=pthread_self();
	sigprocmask(SIG_BLOCK, &lBlock_cc, NULL);
	threads_count=0;
	CreateSocket(port, &sock);
	signal(SIGINT, sigHandler);
	
	while ((isEnd==0) && (threads_count<MAX_CLIENTS)) {
	
	    ListenOnSocket(sock, &newSock);
	    printf("Listener: Got connection! Connection number |%d| \n", threads_count+1);
	    sigprocmask(SIG_BLOCK, &lBlock_cc, NULL);
	    thread_sockets[threads_count]=newSock;
	
	    if ((errorCode=pthread_create(&pthreads[threads_count], NULL, createServ, &thread_sockets[threads_count]))!=0) {
		printf("Listener: THREAD creation FAILED. Failed thread number=|%d|\n", threads_count);
		printf("Listener: pthread_create failed with %s\n", strerror(errorCode)); 
		for (i=0;i<threads_count;i++) {
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			printf("Listener: ERROR - failed to join thread: %s\n", strerror(errorCode));
			exit(errorCode);
		    }   
		    if (status!=0) {
			printf("Listener: Server thread nr. |%d| terminated with error\n", i);
			exit(1);
		    }
		    else {
			printf("Listener: JOINED thread nr. |%d|\n", i);
		    } 
		}  		 	    
	    }
	    else {
		printf("Listener: THREAD creation SUCCESSFUL -> thread nr. |%d|\n", i);
		threads_count++;
	    }
	    printf("Listener: THREAD handled\n");
	    sigprocmask(SIG_UNBLOCK, &lBlock_cc, NULL);
	}
	//TODO - join some, create some new - MANAGE THREADS
	
	for (i=0;i<threads_count;i++) {
	    if ((errorCode=pthread_join(pthreads[threads_count], (void **)&status))!=0) { //STATUS !!
		printf("Listener: ERROR - failed to join thread: %s\n", strerror(errorCode));
		exit(errorCode);
	    }   
	    if (status!=0) {
		printf("Listener: Server thread nr. |%d| terminated with error\n", status);
		exit(1);
	    } 
	}  
	printf("Listener: All threads terminated normally.. returning..\n");

//	    return CreateServer(newSock);
	    //CloseSocket(sock);
	
	return 0;

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




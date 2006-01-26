
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Listener.h"
#include "Server.h"

#define LOCK_PATH_KEY "lockpath"

//using namespace Listener;

Server *srvs[MAX_CLIENTS];
pthread_t pthreads[MAX_CLIENTS];
int threads_count;
Listener *ls;
jmp_buf j;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

Listener::Listener()
{
    memset(thread_sockets, '\0', MAX_CLIENTS);
    threads_count=0;
    ls=this;
}

Listener::~Listener()
{
}

int Listener::Lock(SBQLConfig *config)
{
  string lockPath;
  int errCode = 0;
  struct stat statBuf;
  int fileDes;

  if((errCode=config->getString( LOCK_PATH_KEY, lockPath ))!=0) {
  	printf("[Listener.Lock]-->Error while reading config: %d\n", errCode); 
  	return errCode;
  }

  if( ::stat( lockPath.c_str(), &statBuf)==0) {
    printf("[Listener.Lock]-->ERROR: Can't run server - backup is running. If not, delete file '%s'.\n", lockPath.c_str());
    return -1; //TODO Error
  } else { 
    if((fileDes = ::open( lockPath.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR))<0)
    {
      printf("[Listener.Lock]-->Can't create file '%s': %s\n", lockPath.c_str(), strerror(errno));
      return -1; //TODO Error
    }
  }
  ::close(fileDes);
  printf("[Listener.Lock]-->Done\n");
  return errCode;
}

int Listener::Unlock(SBQLConfig *config)
{
  string lockPath;
  int errCode = 0;

  config->getString(LOCK_PATH_KEY, lockPath);

  if(::unlink(lockPath.c_str())<0)
  {
    printf("[Listener.Unlock]-->Can't delete lock file '%s': %s\n", lockPath.c_str(), strerror(errno));
    return errno;
  }
  printf("[Listener.Unlock]-->Done\n");
  return errCode;
}

int Listener::CreateServer(int sock) {
	Server *srv = new Server(sock); 	
	printf("Listener: running new server thread \n");
	return srv->Run();
}

void *exitPoint() {
    //pthread_lock(&mut);
	//TODO Update threads_count etc
    //pthread_unlock(&mut);
    pthread_exit((void *) 0);
}

    
//LOCK MUTEX BEFORE USING THIS ONE!
int getMyIndex(int pt_id) {
    int i;
    i=0;
    
    while (i<threads_count) {
    	//printf("INDEX: %d, Value %d, my pt_id %d\n", i, (int)(pthreads[i]), pt_id );
	if (((int)(pthreads[i]))==pt_id) {
	    printf("[Listener.getMyIndex]-->Found index\n");
	    return i;
	}
	i++;
    }
    printf("[Listener.getMyIndex]--> ERROR: missing entry in ptreads for this thread!\n");
    return -1;    
}


void sigHandler(int sig) {
    pthread_t pself; 
    int i;
    int errorCode;
    int status;
    
    pself=pthread_self();
    printf("[Listener-sigHandler]--> my pthread_id is %d and ", (int)pself);
    if (pself==pthread_master_id) {
	printf(".. I am the Listener thread \n");
	for (i=0;i<threads_count;i++) {
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			printf("[Listener-sigHandler-Master]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
			exit(errorCode);
		    }   
		    if (status!=0) {
			printf("[Listener-sigHandler-Master]--> Server thread nr. |%d| terminated with error\n", i);
			longjmp(j,1); 
		    }
		    else {
			printf("[Listener-sigHandler-Master]--> JOINED thread nr.|%d| with pthread_id |%d|\n", i, (int)pthreads[i]);
		    } 
	}
	printf("[Listener-sigHandler-Master]--> All threads joined - SUCCESS! Preparing to JUMP \n");
	longjmp(j, 1);
	return; 
    }
    else {
	printf(".. I am a Server thread\n");
	// TODO clean mess
	pthread_mutex_lock(&mut);
	
	status=getMyIndex(int(pself));
	if (status<0) {    
	    pthread_mutex_unlock(&mut);
	    printf("[Listener-sigHandler-Thread%d]-->index not found \n", (int)pself); //TODO args!
	    exitPoint();
	}
	srvs[status]->SExit(0);	
	pthread_mutex_unlock(&mut);
	
	printf("[Listener-sigHandler-Thread%d]--> Exiting \n", (int)pself); 
	return;
	//exitPoint();
    }	
}    

void *createServ(void *arg) {
    int socket=*(int *)arg;
    int res;
    printf("[Listener-createServ]--> New server thread reporting. My socket number is |%d|\n", socket);
    if (socket<0) {
	printf("[Listener-createServ]--> .. which is really unfortunate. I must quit now\n");
	pthread_exit((void *)socket);
    }	
    
    Server *srv = new Server(socket);
    
    // TODO update thread global data
    pthread_mutex_lock(&mut);
	if ((res=getMyIndex((int)pthread_self()))<0) {
	    pthread_mutex_unlock(&mut);
	    pthread_exit((void *)res);
	}
	printf("[Listener-createServ]--> Putting my server object into global table..(index=%d)\n", res);
	srvs[res]=srv;
    pthread_mutex_unlock(&mut);	
  
    printf("[Listener-createServ]--> Executing server code now.. \n");
    res=srv->Run();
    printf("[Listener-createServ]--> Server returned |%d|, exiting \n", res); 
    pthread_exit((void *)res);
}

int Listener::Start(int port) {
	
	int sock;
	int newSock=-1;
	int isEnd=0;
	int errorCode=0;
	int i=0;
	int status;
	
	
	printf("[Listener.Start]--> Initializing objects.. \n");
	
	SBQLConfig* config = new SBQLConfig("Server");
	errorCode=config->init("Server.conf");
	if (errorCode!=0) 
		printf("Listener.Start]--> Config init failed with %d\n", errorCode);
	
	ErrorConsole con("Server");
	errorCode=con.init(1);
	if (errorCode!=0) 
		printf("Listener.Start]--> ErrorConsole init failed with %d\n", errorCode);
	
	lCons=&con;
	lConf=config;	
	Lock(config);
	//Lock(lConf);
	
	sigset_t lBlock_cc;
	sigemptyset(&lBlock_cc);
	sigaddset(&lBlock_cc, SIGINT);	
	
	pthread_master_id=pthread_self();
	threads_count=0;
	if ((errorCode=CreateSocket(port, &sock))!=0) {
	    printf("[Listener.Start]--> Error in Create Socket: %d\n", errorCode);
	    return errorCode;
	}
	signal(SIGINT, sigHandler);
	
	if (setjmp(j)!=0) {
	    printf("[Listener.Start]--> Jumped.. closing socket..\n");
	    Unlock(lConf);
	    errorCode=CloseSocket(sock);
	    //printf("[Listener.Start]--> Returning with %d\n", errorCode);
	    return errorCode;
	} 
	
	while ((isEnd==0) && (threads_count<MAX_CLIENTS)) {
	
	    ListenOnSocket(sock, &newSock);
	    printf("[Listener.Start]--> Got connection! Connection number |%d| \n", threads_count+1);
	    sigprocmask(SIG_BLOCK, &lBlock_cc, NULL);
	    pthread_mutex_lock(&mut);
	    thread_sockets[threads_count]=newSock;
	
	    if ((errorCode=pthread_create(&pthreads[threads_count], NULL, createServ, &thread_sockets[threads_count]))!=0) {
		printf("[Listener.Start]--> THREAD creation FAILED. Failed thread number=|%d|\n", threads_count);
		printf("[Listener.Start]--> PTHREAD_CREATE FAILED with %s\n", strerror(errorCode)); 
		for (i=0;i<threads_count;i++) {
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			printf("[Listener.Start]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
			pthread_mutex_unlock(&mut);
			exit(errorCode);
		    }   
		    if (status!=0) {
			printf("[Listener.Start]--> Server thread nr. |%d| terminated with error\n", i);
			pthread_mutex_unlock(&mut);
			exit(status);
		    }
		    else {
			printf("[Listener.Start]--> JOINED thread nr. |%d|\n", i);
		    } 
		}  		 	    
	    }
	    else {
		printf("[Listener.Start]--> THREAD creation SUCCESSFUL -> thread nr. |%d|\n", i);
		threads_count++;
	    }
	    pthread_mutex_unlock(&mut);
	    printf("[Listener.Start]--> THREAD handled\n");
	    sigprocmask(SIG_UNBLOCK, &lBlock_cc, NULL);
	}
	//TODO - join some, create some new - MANAGE THREADS
	
	for (i=0;i<threads_count;i++) {
	    if ((errorCode=pthread_join(pthreads[threads_count], (void **)&status))!=0) { //STATUS !!
		printf("[Listener.Start]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
		exit(errorCode);
	    }   
	    if (status!=0) {
		printf("[Listener.Start]--> Server thread nr. |%d| terminated with error\n", status);
		exit(1); //TODO ERROR ?
	    } 
	}  
	printf("[Listener.Start]--> All threads terminated normally.. returning..\n");
	CloseSocket(sock);
	
	return 0;

}

int Listener::getConsole(ErrorConsole *&cons) {
	cons=lCons;
	return 0;
}

int Listener::getConfig(SBQLConfig *&conf) {
	conf=lConf;
	return 0;
}



int main(int argc, char* argv[]) {
	int port = 6543;
	if (argc>1) 
		port = atoi(argv[1]);
	printf("[Listener.MAIN] Running port ----> %d\n", port);

	int res;
	Listener *ls = new Listener();
	
	if ((res=(ls->Start(port)))!=0) {
	    printf("[Listener.MAIN]--> ends with ERROR: %d \n", res);
	    return res;
	}
	printf("[Listener.MAIN]--> *********END********* \n");
	return 0;	
}




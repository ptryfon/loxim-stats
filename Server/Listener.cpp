
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
    lCons = new ErrorConsole("Server");
    memset(thread_sockets, '\0', MAX_CLIENTS);
    threads_count=0;
    ls=this;
}

Listener::~Listener()
{
	if (lCons != NULL)
		delete lCons;
}

int Listener::Lock()
{
  string lockPath;
  int errCode = 0;
  struct stat statBuf;
  int fileDes;
  SBQLConfig config("Server");

  if((errCode=config.getString( LOCK_PATH_KEY, lockPath ))!=0) {
  	lCons->printf("[Listener.Lock]-->Error while reading config: %d\n", errCode); 
  	return errCode;
  }

  if( ::stat( lockPath.c_str(), &statBuf)==0) {
    lCons->printf("[Listener.Lock]-->ERROR: Can't run server - backup is running. If not, delete file '%s'.\n", lockPath.c_str());
    return -1; //TODO Error
  } else { 
    if((fileDes = ::open( lockPath.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR))<0)
    {
      lCons->printf("[Listener.Lock]-->Can't create file '%s': %s\n", lockPath.c_str(), strerror(errno));
      return -1; //TODO Error
    }
  }
  ::close(fileDes);
  *lCons << "[Listener.Lock]-->Done";
  return errCode;
}

int Listener::Unlock()
{
  string lockPath;
  int errCode = 0;
  SBQLConfig config("Server");

  config.getString(LOCK_PATH_KEY, lockPath);

  if(::unlink(lockPath.c_str())<0)
  {
    lCons->printf("[Listener.Unlock]-->Can't delete lock file '%s': %s\n", lockPath.c_str(), strerror(errno));
    return errno;
  }
  *lCons << "[Listener.Unlock]-->Done";
  return errCode;
}

int Listener::CreateServer(int sock) {
	Server *srv = new Server(sock); 	
	*lCons << "Listener: running new server thread";
	return srv->Run();
}

void *exitPoint() {
    //pthread_lock(&mut);
	//TODO Update threads_count etc
    //pthread_unlock(&mut);
    pthread_exit((void *) 0);
}

    
//LOCK MUTEX BEFORE USING THIS ONE!
int getMyIndex(int pt_id, ErrorConsole *ec) {
    int i;
    i=0;
    
    while (i<threads_count) {
    	//lCons->printf("INDEX: %d, Value %d, my pt_id %d\n", i, (int)(pthreads[i]), pt_id );
	if (((int)(pthreads[i]))==pt_id) {
	    *ec << "[Listener.getMyIndex]-->Found index";
	    return i;
	}
	i++;
    }
    *ec << "[Listener.getMyIndex]--> ERROR: missing entry in ptreads for this thread!";
    return -1;    
}


void sigHandler(int sig) {
    pthread_t pself; 
    int i;
    int errorCode;
    int status;
    ErrorConsole ec("Server");
    
    pself=pthread_self();
    ec.printf("[Listener-sigHandler]--> my pthread_id is %d and ", (int)pself);
    if (pself==pthread_master_id) {
	ec << ".. I am the Listener thread";
	for (i=0;i<threads_count;i++) {
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			ec.printf("[Listener-sigHandler-Master]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
			exit(errorCode);
		    }   
		    if (status!=0) {
			ec.printf("[Listener-sigHandler-Master]--> Server thread nr. |%d| terminated with error\n", i);
			longjmp(j,1); 
		    }
		    else {
			ec.printf("[Listener-sigHandler-Master]--> JOINED thread nr.|%d| with pthread_id |%d|\n", i, (int)pthreads[i]);
		    } 
	}
	ec << "[Listener-sigHandler-Master]--> All threads joined - SUCCESS! Preparing to JUMP";
	longjmp(j, 1);
	return; 
    }
    else {
	ec << ".. I am a Server thread";
	// TODO clean mess
	pthread_mutex_lock(&mut);
	
	status=getMyIndex(int(pself), &ec);
	if (status<0) {    
	    pthread_mutex_unlock(&mut);
	    ec.printf("[Listener-sigHandler-Thread%d]-->index not found \n", (int)pself); //TODO args!
	    exitPoint();
	}
	srvs[status]->SExit(0);	
	pthread_mutex_unlock(&mut);
	
	ec.printf("[Listener-sigHandler-Thread%d]--> Exiting \n", (int)pself); 
	return;
	//exitPoint();
    }	
}    

void *createServ(void *arg) {
    int socket=*(int *)arg;
    int res;
    ErrorConsole ec("Server");
    
    ec.printf("[Listener-createServ]--> New server thread reporting. My socket number is |%d|\n", socket);
    if (socket<0) {
	ec << "[Listener-createServ]--> .. which is really unfortunate. I must quit now";
	pthread_exit((void *)socket);
    }	
    
    Server *srv = new Server(socket);

    // watek zaczynal sie wykonywac zanim thread_index zostal zwiekszony
    // wiec myslal ze go nie ma, sleep(1) wystarcza w wiekszosci przypadkow
    sleep(1);    
    // TODO update thread global data
    pthread_mutex_lock(&mut);
	if ((res=getMyIndex((int)pthread_self(),&ec))<0) {
	    pthread_mutex_unlock(&mut);
	    pthread_exit((void *)res);
	}
	ec.printf("[Listener-createServ]--> Putting my server object into global table..(index=%d)\n", res);
	srvs[res]=srv;
    pthread_mutex_unlock(&mut);	
  
    ec << "[Listener-createServ]--> Executing server code now..";
    res=srv->Run();
    ec.printf("[Listener-createServ]--> Server returned |%d|, exiting \n", res); 
    pthread_exit((void *)res);
}

int Listener::Start(int port) {
	
	int sock;
	int newSock=-1;
	int isEnd=0;
	int errorCode=0;
	int i=0;
	int status;
	
	
	*lCons << "[Listener.Start]--> Initializing objects..";
	
	if ((errorCode=Lock())!=0)
	    exit(errorCode);
	
	
	sigset_t lBlock_cc;
	sigemptyset(&lBlock_cc);
	sigaddset(&lBlock_cc, SIGINT);	
	
	pthread_master_id=pthread_self();
	threads_count=0;
	if ((errorCode=CreateSocket(port, &sock))!=0) {
	    lCons->printf("[Listener.Start]--> Error in Create Socket: %d\n", errorCode);
	    return errorCode;
	}
	signal(SIGINT, sigHandler);
	
	lCons->printf("[Listener.Start]--> Initializing Log manager and Store manager \n");
	LogManager *lm = new LogManager();
	lm->init();
	DBStoreManager *sm = new DBStoreManager();
	sm->init(lm);
	TransactionManager::getHandle()->init(sm, lm);
	sm->setTManager(TransactionManager::getHandle());
	lCons->printf("[Listener.Start]--> Starting Store manager.. \n");
	sm->start();
	lCons->printf("[Listener.Start]--> Starting Log manager.. \n");
	lm->start(sm);
	
	if (setjmp(j)!=0) {
	    *lCons << "[Listener.Start]--> Jumped.. Stopping Store manager and  closing socket..";
	    sm->stop();
	    unsigned idx;
	    lm->shutdown(idx);
	    delete TransactionManager::getHandle();
	    Unlock();
	    errorCode=CloseSocket(sock);
	    //lCons->printf("[Listener.Start]--> Returning with %d\n", errorCode);
	    return errorCode;
	} 
	
	
	while ((isEnd==0) && (threads_count<MAX_CLIENTS)) {
	
	    ListenOnSocket(sock, &newSock);
	    lCons->printf("[Listener.Start]--> Got connection! Connection number |%d| \n", threads_count+1);
	    sigprocmask(SIG_BLOCK, &lBlock_cc, NULL);
	    pthread_mutex_lock(&mut);
	    thread_sockets[threads_count]=newSock;
	    pthread_mutex_unlock(&mut);    
	
	    if ((errorCode=pthread_create(&pthreads[threads_count], NULL, createServ, &thread_sockets[threads_count]))!=0) {
		lCons->printf("[Listener.Start]--> THREAD creation FAILED. Failed thread number=|%d|\n", threads_count);
		lCons->printf("[Listener.Start]--> PTHREAD_CREATE FAILED with %s\n", strerror(errorCode)); 
		for (i=0;i<threads_count;i++) {
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			lCons->printf("[Listener.Start]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
			exit(errorCode);
		    }   
		    if (status!=0) {
			lCons->printf("[Listener.Start]--> Server thread nr. |%d| terminated with error\n", i);
			exit(status);
		    }
		    else {
			lCons->printf("[Listener.Start]--> JOINED thread nr. |%d|\n", i);
		    } 
		}  		 	    
	    }
	    else {
		lCons->printf("[Listener.Start]--> THREAD creation SUCCESSFUL -> thread nr. |%d|\n", i);
		threads_count++;
	    }

	    *lCons << "[Listener.Start]--> THREAD handled";
	    sigprocmask(SIG_UNBLOCK, &lBlock_cc, NULL);
	}
	//TODO - join some, create some new - MANAGE THREADS
	
	for (i=0;i<threads_count;i++) {
	    if ((errorCode=pthread_join(pthreads[threads_count], (void **)&status))!=0) { //STATUS !!
		lCons->printf("[Listener.Start]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
		exit(errorCode);
	    }   
	    if (status!=0) {
		lCons->printf("[Listener.Start]--> Server thread nr. |%d| terminated with error\n", status);
		exit(1); //TODO ERROR ?
	    } 
	}  
	*lCons << "[Listener.Start]--> All threads terminated normally.. returning..";
	CloseSocket(sock);
	
	return 0;

}



int main(int argc, char* argv[]) {
	int port = 6543;
	int errorCode;
	if (argc>1) 
		port = atoi(argv[1]);

	SBQLConfig config("Server");
	errorCode = config.init("Server.conf");
	if (errorCode!=0) {
		fprintf(stderr, "Listener.Start]--> Config init failed with %d\n", errorCode);
		exit(1);
	}
	
	ErrorConsole con("Server");
	errorCode=con.init(1);
	if (errorCode!=0) {
		fprintf(stderr, "Listener.Start]--> ErrorConsole init failed with %d\n", errorCode);
		exit(2);
	}
	
	con.printf("[Listener.MAIN] Running port ----> %d\n", port);

	int res;
	Listener *ls = new Listener();
	
	if ((res=(ls->Start(port)))!=0) {
	    con.printf("[Listener.MAIN]--> ends with ERROR: %d \n", res);
	    return res;
	}
	con << "[Listener.MAIN]--> *********END*********";
	con.free();
	config.free();
	return 0;	
}

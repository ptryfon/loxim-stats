
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
#include "../Indexes/IndexManager.h"

#define LOCK_PATH_KEY "lockpath"

Server *srvs[MAX_CLIENTS];
pthread_t pthreads[MAX_CLIENTS];
pthread_t pulseThreads[MAX_CLIENTS];
int takenFields[MAX_CLIENTS];
int max_thread_index;
int running_threads_count;
int pulseThread_index;
Listener *ls;
jmp_buf j;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

Listener::Listener()
{
    lCons = new ErrorConsole("Server");
    memset(thread_sockets, '\0', MAX_CLIENTS);
    memset(thread_sockets_2, '\0', MAX_CLIENTS);
    memset(takenFields, 0, MAX_CLIENTS);
    memset(pulseThreads, 0, MAX_CLIENTS);
    memset(pthreads, 0, MAX_CLIENTS);
    max_thread_index=-1;
    running_threads_count=0;
    pulseThread_index=0;
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
	//Server *srv = new Server(sock); 	
	//*lCons << "Listener: running new server thread";
	//return srv->Run();
	return -1;
}

void *exitPoint() {
    pthread_exit((void *) 0);
}

    
//LOCK MUTEX BEFORE USING THIS ONE!
int getMyIndex(int pt_id, ErrorConsole *ec) {
    int i;
    i=0;
    
    while (i<max_thread_index+1) {
    	//lCons->printf("INDEX: %d, Value %d, my pt_id %d\n", i, (int)(pthreads[i]), pt_id );
	if (((int)(pthreads[i]))==pt_id) {
	    *ec << "[Listener.getMyIndex]-->Found index";
	    return i;
	}
	i++;
    }
    i=0;
    while (i<max_thread_index+1) {
    	//lCons->printf("INDEX: %d, Value %d, my pt_id %d\n", i, (int)(pthreads[i]), pt_id );
	if (((int)(pulseThreads[i]))==pt_id) {
	    *ec << "[Listener.getMyIndex]-->Found index (pulseChecker thread)";
	    return MAX_CLIENTS+i;
	}
	i++;
    }
    *ec << "[Listener.getMyIndex]--> ERROR: missing entry in ptreads for this thread!";
    return -1;    
}

int getPTindex(pthread_t pt_id) {
    int i=0;
    ErrorConsole ec("getPTIndex");
    while (i<max_thread_index+1) {
	if ((int)pthreads[i] == (int)pt_id)
	    return i;
	i++;	
    }
    return -1;    
}

int emptyPocket() {
    ErrorConsole ec("emptyPocket");
    pthread_t pself;
    pself = pthread_self();
    int res;
    //ec.printf("Max=%d\n", max_thread_index);
    res=getPTindex(pself);
    if (res==-1)
	return -1;
    takenFields[res]=0;
    if (res==max_thread_index)
	max_thread_index--;
    return res;	
}

int pickPocket() {
    int i=0;
    for (i=0;i<max_thread_index+1;i++) {
	if (takenFields[i]==0) {
	    takenFields[i]=1;
	    return i;
	}
    }
    takenFields[max_thread_index+1]=1;
    return max_thread_index+1;
}

void sigHandler(int sig) {
    pthread_t pself; 
    int i;
    int errorCode;
    int status;
    ErrorConsole ec("Server");
    
    pself=pthread_self();
    ec.printf("[Listener-sigHandler]--> my pthread_id is %d and \n", (int)pself);
    if (pself==pthread_master_id) {
	ec << ".. I am the Listener thread";
	for (i=0;i<max_thread_index+1;i++) {
		    ec.printf("[Listenr-sigHandler-Master]--> Joining thread at %d (max %d)\n",  i, max_thread_index);
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			ec.printf("[Listener-sigHandler-Master]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
			exit(errorCode);
		    }   
		    if (status!=0) {
			ec.printf("[Listener-sigHandler-Master]--> Server thread nr %d terminated with error\n", i);
			longjmp(j,1); 
		    }
		    else {
			ec.printf("[Listener-sigHandler-Master]--> JOINED thread nr %d with pthread_id |%d|\n", i, (int)pthreads[i]);
		    } 
	}
	ec << "[Listener-sigHandler-Master]--> All threads joined - SUCCESS! Preparing to JUMP";
	longjmp(j, 1);
	return; 
    }
    else {
	ec << ".. I am a Server/PulseChecker thread";
	// TODO clean mess
	pthread_mutex_lock(&mut);
	
	status=getMyIndex(int(pself), &ec);
	if (status<0) {    
	    pthread_mutex_unlock(&mut);
	    ec.printf("[Listener-sigHandler-Thread%d]-->index not found \n", (int)pself); //TODO args!
	    exitPoint();
	}
	if (status>MAX_CLIENTS-1) {
	    pthread_mutex_unlock(&mut);
	    ec.printf("[Listener-sigHandler-Thread%d]-->I am a pulsechecker thread\n", (int)pself);
	    exitPoint();
	}
	else
	    srvs[status]->SExit(0);	
	int ecNo=emptyPocket();
	ec.printf("[Listener-sighandler-Thread%d]--> ... %d\n", (int)pself, ecNo);
	pthread_mutex_unlock(&mut);
	
	ec.printf("[Listener-sigHandler-Thread%d]--> Exiting \n", (int)pself); 
	return;
	//exitPoint();
    }	
}    

void *createServ(void *arg) {
    doubleSock ds = *(doubleSock *)arg;
    //int socket=*(int *)arg;
    int res;
    ErrorConsole ec("Server");
    int socket = ds.sock;
    int pcSocket = ds.pcSock;
    
    ec.printf("[Listener-createServ]--> New server thread reporting. My socket numbers are |%d|%d|\n", socket, pcSocket);
    if (socket<0) {
	ec << "[Listener-createServ]--> .. which is really unfortunate. I must quit now\n";
	pthread_exit((void *)socket);
    }	
    //ec.printf("[Listener-createServ]--> New server thread reporting. My PC socket number is |%d|\n", pcSocket);
    if (pcSocket<0) {
	ec << "[Listener-createServ]--> bad PC socket, quitting\n";
	pthread_exit((void *)pcSocket);
    }
    
    Server *srv = new Server(socket, pcSocket);

    // watek zaczynal sie wykonywac zanim thread_index zostal zwiekszony
    // wiec myslal ze go nie ma, sleep(1) wystarcza w wiekszosci przypadkow
    sleep(1);    
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
    ec.printf("[Listener-createServ]--> Server returned |%d|, locking mutex \n", res);
    pthread_mutex_lock(&mut);
    int res2=emptyPocket();
    ec.printf("[Listener-createServ]--> Server cleared at index %d \n", res2);
    pthread_mutex_unlock(&mut);
    Listener::CloseSocket(socket);
    ec.printf("[Listener-createServ]--> Socket closed, mutex unlocked - thread exiting\n"); 
    pthread_exit((void *)res);
}

int Listener::Start(int port) {
	
	int sock;
	int newSock=-1;
	
	int pcSock;
	int newPcSock=-2;
	
	int isEnd=0;
	int errorCode=0;
	int i=0;
	int status;
	
        LogManager::checkForBackup();
	*lCons << "[Listener.Start]--> Initializing objects..";
	
	if ((errorCode=Lock())!=0)
	    exit(errorCode);
	
	
	sigset_t lBlock_cc;
	sigemptyset(&lBlock_cc);
	sigaddset(&lBlock_cc, SIGINT);	
	
	pthread_master_id=pthread_self();
	max_thread_index=-1;
	running_threads_count=0;
	pulseThread_index=0;
	if ((errorCode=CreateSocket(port, &sock))!=0) {
	    lCons->printf("[Listener.Start]--> Error in Create Socket: %d\n", errorCode);
	    return errorCode;
	}
	
	if ((errorCode=CreateSocket(port+1, &pcSock))!=0) {
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
	lCons->printf("[Listener.Start]--> Starting Index manager.. \n");
	Indexes::IndexManager::init(LogManager::isCleanClosed());
	
	if (setjmp(j)!=0) {
	    *lCons << "[Listener.Start]--> Jumped.. Stopping Store manager and  closing socket..";
	    sm->stop();
	    unsigned idx;
	    lm->shutdown(idx);
	    Indexes::IndexManager::shutdown();
	    delete TransactionManager::getHandle();
	    Unlock();
	    errorCode=CloseSocket(sock);
	    //lCons->printf("[Listener.Start]--> Returning with %d\n", errorCode);
	    return errorCode;
	} 
	
	
	while ((isEnd==0) && (max_thread_index<MAX_CLIENTS)) {
	    
	    ListenOnSocket(sock, &newSock);
	    lCons->printf("[Listener.Start]--> Got connection (%d)!\n", newSock);
	    	    
	    sigprocmask(SIG_BLOCK, &lBlock_cc, NULL);
	    pthread_mutex_lock(&mut);
	    int ff_index=pickPocket();
	    lCons->printf("[Listener.Start]--> Max=%d, Current=%d\n", max_thread_index, ff_index);
	    //thread_sockets[ff_index]=newSock;
	    doubleSock dS;
	    dS.sock = newSock;
	    dS.pcSock = pcSock;
	    thread_sockets_2[ff_index]=dS;
	    pthread_mutex_unlock(&mut);    
	    pthread_mutex_lock(&mut);
	
	    if ((errorCode=pthread_create(&pthreads[ff_index], NULL, createServ, &thread_sockets_2[ff_index]))!=0) {
		lCons->printf("[Listener.Start]--> THREAD creation FAILED. Failed thread number=|%d|\n", ff_index);
		lCons->printf("[Listener.Start]--> PTHREAD_CREATE FAILED with %s\n", strerror(errorCode)); 
		for (i=0;i<max_thread_index+1;i++) {
		    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
			lCons->printf("[Listener.Start]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
			pthread_mutex_unlock(&mut);
			exit(errorCode);
		    }   
		    if (status!=0) {
			lCons->printf("[Listener.Start]--> Server thread nr. |%d| terminated with error\n", i);
			pthread_mutex_unlock(&mut);
			exit(status);
		    }
		    else {
			lCons->printf("[Listener.Start]--> JOINED thread nr. |%d|\n", i);
		    } 
		}  		 	    
	    }
	    else {
		lCons->printf("[Listener.Start]--> THREAD creation SUCCESSFUL -> thread nr. |%d|\n", i);
		//pthread_mutex_lock(&mut);
		if (ff_index>max_thread_index)
		    max_thread_index=ff_index;
		running_threads_count++;
		//pthread_mutex_unlock(&mut);
	    }

	    pthread_mutex_unlock(&mut);
	    *lCons << "[Listener.Start]--> THREAD handled";
	    sigprocmask(SIG_UNBLOCK, &lBlock_cc, NULL);
	    
	}

	for (i=0;i<max_thread_index+1;i++) {
	    if ((errorCode=pthread_join(pthreads[i], (void **)&status))!=0) { //STATUS !!
		lCons->printf("[Listener.Start]--> ERROR - failed to join thread: %s\n", strerror(errorCode));
		exit(errorCode);
	    }   
	    if (status!=0) {
		lCons->printf("[Listener.Start]--> Server thread nr. |%d| terminated with error\n", status);
		exit(1); //TODO ERROR ?
	    } 
	}  
	*lCons << "[Listener.Start]--> All threads terminated normally.. returning..";
	CloseSocket(newSock);
	
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

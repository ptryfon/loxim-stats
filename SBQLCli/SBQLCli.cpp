#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <string>
#include <signal.h>

#include <unistd.h>
#include "../Driver/DriverManager.h"
#include "../Driver/Result.h"

/** If defined the $ans parameter holds the result of the last query. */
//#define ANS_PARAMETER_ADDITION_ACTIVE
/** Client version number. */
#define VERSION "1.1"
/** If defined the program prints a verbose output. */
//#define DEBUG

using namespace std;
using namespace Driver;

pthread_cond_t conditional_v = PTHREAD_COND_INITIALIZER;
/** Pulse thread id. */
pthread_t pulseT;
/** Global connection variable. */
Connection *con;
pthread_mutex_t cliMut = PTHREAD_MUTEX_INITIALIZER;
/** This structure holds connection information. */
struct pInfo {
    /** Hostname. */
    char *host;
    /** Login. */
    const char *login;
    /** Password. */
    const char *passwd;
    /** Port number. */
    int port;
    /** Thread id of self. */
    pthread_t id;
};

void read_and_execute(istream &in, Connection *con );
void execute_query(Connection *con, string &query, Result** lastResult);
void printHead();
void printUsage();

/**
 * This functions prints out the message on standard output, if descriptor 0 (zero) 
 * refers to terminal. 
 *
 * @param str Message to be printed out.
 */
void printMsg (string str)
{
  if (isatty (0))
    cout << str;
}

/**
 * SBQLCli signal handler.
 * @param sig Int signal id.
 */
void cliSigHandler(int sig) {
  int errorCode;
  int status;
  if (pthread_self()==pulseT)
    exit(0);
  else { 
    errorCode = pthread_join(pulseT, (void**)&status);
    if (errorCode!=0) {
        printf("Error: Failed to join cliPulse thread - %s\n", strerror(errorCode));
        exit(errorCode);
    }
    exit(0);
  }
}

/**
 * <i>Start routine</i> function for the new thread. Function connects to the server.
 * @param arg Reference to pInfo structure.
 */
void *pulseRun(void *arg) {
    pInfo pI = *(pInfo *)arg;
    Connection *pcCon; 
    pcCon = DriverManager::getConnection(pI.host, pI.port, pI.login, pI.passwd, 0);
    pthread_mutex_lock(&cliMut);
    pthread_cond_wait(&conditional_v, &cliMut);
    pthread_mutex_unlock(&cliMut);
    printMsg("\nExiting\n");
    delete pcCon;
    //sigprocmask(SIG_UNBLOCK, &block_sigs, NULL);
    pthread_exit((void *)0); 
}

/**
 * Function reads login from <i>cin</i> and saved it to the <i>login</i> variable.
 *
 * @param &login Variable set with this method.
 */ 
void read_login(string &login) {
    printMsg ("Login: "); 
    getline(cin, login);    
}

/**
 * Function reads password from <i>cin</i> and saved it to the <i>passwd</i> variable.
 * There's no echo shown on the screen.
 *
 * @param &passwd Variable set with this method.
 */ 
void read_passwd(string &passwd) {
    if (isatty(0)) {
	printMsg ("Password: "); 
	system("stty -echo");
	getline(cin, passwd);
	system("stty echo");    
    }
}
/**
 * This method allows clean exit, removes "pulse thread" and closes the connection.
 *
 * @param code Int value passed to exit(int code) function.
 */
void goodExit(int code) {
    pthread_mutex_lock(&cliMut);
    pthread_cond_signal(&conditional_v);
    pthread_mutex_unlock(&cliMut);
    pthread_join(pulseT, NULL);
     
    delete con;
    exit(code);

}

/**
 * Start function for the client program.
 */
int main (int argc, char *argv[])
{

  char *host = "localhost";
  int port = 6543;
  string login, passwd;

  /* args things */
  if (argc > 1) {
    if (argv[1][0] == '-') {
      for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        if (arg=="-?" || arg=="--help") 
        {
          printUsage();
          exit(0);
        }
        if (arg.substr(0,2)=="-l" || arg.substr(0,2)=="-u" ) {
          login=arg.substr(2);
        }
        if (arg.substr(0,2)=="-p") {
          passwd=arg.substr(2);
        }
        if (arg.substr(0,2)=="-h") {
          host=(char*)arg.substr(2).c_str();
        }
        if (arg.substr(0,2)=="-P") {
          port=atoi(arg.substr(2).c_str());
        }
      }
    } else {
    //old school -- compatibility issues
      if (argc >= 2) {
        host = argv[1];
      }
      if (argc == 3) {
        port = atoi (argv[2]);
      }

      if (argc > 3) {
        printUsage();
        exit (1);
      }
    }
  }

  printHead();

  if (login == "") {
    read_login(login);
    read_passwd(passwd);
    if ("" == passwd) passwd = "not_a_password";
    printMsg("\n");
  }

  /* establishing connection */
  pthread_t myTid = pthread_self();
  int error;
  pInfo cI;
  cI.host = host;
  cI.port = port;
  cI.login = login.c_str();
  cI.passwd = passwd.c_str(); 
  cI.id = myTid;

  //getting connection
  try {
#ifdef DEBUG
    cerr << "Getting connection..." << endl;
#endif
    con = DriverManager::getConnection (cI.host, cI.port, cI.login, cI.passwd, 1);
#ifdef DEBUG
    cerr << "Got main connection, getting pulse connection..." << endl;
#endif
  } catch (ConnectionException e) {
    cerr << e << endl;
    exit(1);
  }

  //getting pulse
  try 
  {
    if ((error = pthread_create(&pulseT, NULL, pulseRun, &cI))!=0) 
    {
      cerr << "thread creation failed with " << error;
    }
#ifdef DEBUG
    cerr << "Got pulse connection, I am " << pthread_self() << endl;
#endif
  } 
  catch (ConnectionException e) 
  {
    cerr << e << endl;
    goodExit(1);
  }

  printMsg ("\n");

  string line;			// one line of a query
  string input;			// whole query (possibly multi line)
  Result *result = new ResultVoid();

  printMsg (" > ");
  try
  {
    while (getline (cin, line))
    {
	/* @file_with_queries */
        if (line[0] == '@') {
    	    const char *filename = line.c_str() + 1;
	    ifstream fin(filename);
	    read_and_execute(fin, con);
	    fin.close();
        }
	if (line[0] != '/')
	  {
	    if (!((line.length () >= 2)
		  && (line[0] == '-') && (line[1] == '-')))
	      {
			input.append (line + " ");
	      }
	    printMsg (" \\ ");
	  }
	else
	{
	    execute_query(con, input, &result);
	}
      }	//while

  }
  catch (ConnectionException e)
  {
    cerr << e << endl;
    goodExit(1);
  }
    
  /* sending abort on exit */
  cerr << endl << "<SBQLCli> auto abort on exit " << endl;
  if (result != NULL) delete result;
  try
  {
    result = con->execute ("abort;");
    cout << *result << endl;
  }
  catch (ConnectionServerException e)
  {
    cerr << e << endl;
  }

  goodExit(0);
}

/**
 * Reads from input stream (file), multiline stream of commands and execute them.
 * 
 * @param &in The input stream.
 * @param con Connection object.
 */
void read_and_execute(istream &in, Connection *con) {
  printMsg ("\n");
  
  string line;			// one line of a query
  string input;			// whole query (possibly multi line)
  Result *result = new ResultVoid();

  printMsg (" > ");
  try
  {
    while (getline (in, line))
      {
        /* @file_with_queries */
        if (line[0] == '@') {
    	    const char *filename = line.c_str() + 1;
	    ifstream fin(filename);
	    read_and_execute(fin, con);
	    fin.close();
        }
	if (line[0] != '/')
	  {
	    if (!((line.length () >= 2)
		  && (line[0] == '-') && (line[1] == '-')))
	      {
			input.append (line + " ");
	      }
	    printMsg (" \\ ");
	  }
	else
	{
	  execute_query(con, input, &result);
	}
      }	//while
  }
  catch (ConnectionException e)
  {
    cerr << e << endl;
    goodExit(1);
  }

};

/** 
 * Function executes single query.
 *
 * @param con Connection object.
 * @param input String with query.
 * @param result Result of the last query, possibly needed by $ans. Cannot be NULL.
 */
void execute_query(Connection *con, string &input, Result **result) {
  //checking input params
  if (*result == NULL) {
    cerr << "SBQLCli: [execute_query]--> assert(result!=NULL) failed." << endl;
    goodExit(1);
  }
  cerr << "<SBQLCli> query: " << input << endl;
  
    try
    {
#ifdef ANS_PARAMETER_ADDITION_ACTIVE 
    cerr << "<SBQLCli> adding last result as a parameter $ans = " 
	<< **result << endl;
    Result* resultTmp;
      Statement* stmt;
      stmt   = con->parse(input.c_str());
      stmt->addParam("$ans", *result);
      resultTmp = con->execute(stmt);
      delete stmt;
      delete *result;
      *result = resultTmp;
#endif
    
#ifndef ANS_PARAMETER_ADDITION_ACTIVE 
      //if (*result != NULL) delete *result;
      *result = con->execute (input.c_str ());
#endif
      cout << **result << endl;
      
    }
  catch (ConnectionServerException e)
    {			//parse error, or smth.
      cerr << e << endl;
    }
  
  input.clear ();
  printMsg (" > ");
  
}

/**
 * Function prints out the client name and verision.
 */
void printHead() {
  printMsg ("SBQLCli ver " + string(VERSION));
  printMsg ("\nCtrl-d to exit \n");
}

/**
 * Function prints out the usage infomation.
 * The output of this function is avalible with <tt>--help</tt> switch from
 * command-line.
 */
void printUsage() {
  printHead();
  printMsg ("Usage: SBQLCli [OPTIONS] \n");
  printMsg ("-?, --help        Display this help and exit.\n");
  printMsg ("-llogin, -ulogin  Login\n");
  printMsg ("-ppassword        Password\n");
  printMsg ("-hlocalhost       Host (default: localhost)\n");
  printMsg ("-P6543            Port (default: 6543)\n");
  printMsg ("\n");
  printMsg ("Example usage:\n");
  printMsg ("> ./SBQLCli -lscott -ptiger -hlocalhost -P6543 \n\n");
}

//didn't know where to put that...

/** 
 * \mainpage LoXiM
 *
 *  This is the generated documentation of LoXiM - an experimental database
 *  management system. 
 *
 *  The main (entry) class of the server is Server::Listener. 
 *
 *  The main (entry) class of an example client is SBQLCli::SBQLCli.
 *
 *  To run the software, simple start the serwer (Server/Listener) 
 *  and then connect to this server by means of the command line client, i.e.
 *  SBQLCli/SBQLCli. The configuration of the server is read from 
 *  file Server.conf.
 */
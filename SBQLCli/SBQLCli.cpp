#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "../Driver/DriverManager.h"
#include "../Driver/Result.h"

#define ANS_PARAMETER_ADDITION_ACTIVE  // $ans parameter holds the result of the last query


using namespace std;
using namespace Driver;

void read_and_execute(istream &in, Connection *con );
void execute_query(Connection *con, string &query, Result** lastResult);

void printMsg (string str)
{
  if (isatty (0))
    cout << str;
}

void read_login(string &login) {
    printMsg ("Login: "); 
    getline(cin, login);    
}

void read_passwd(string &passwd) {
    if (isatty(0)) {
	printMsg ("Password: "); 
	system("stty -echo");
	getline(cin, passwd);
	system("stty echo");    
    }
}

int main (int argc, char *argv[])
{

  char *host = "localhost";
  int port = 6543;

  /* args things */
  if (argc >= 2)
    {
      host = argv[1];
    }

  if (argc == 3)
    {
      port = atoi (argv[2]);
    }

  if (argc > 3)
    {
      cout << "usage: " << argv[0] << " host port " << endl;
      exit (1);
    }

  printMsg ("SBQLCli ver 1.0 \n");
  printMsg ("Ctrl-d exits \n");   

  string login, passwd;
  read_login(login);
  read_passwd(passwd);
  if ("" == passwd) passwd = "not_a_password";  
  /* establishing connection */
  Connection *con;
  try
  {
    //Adam - odkomentuj te jedna linijke:
    //con = DriverManager::getConnection(host, port+1);
    con = DriverManager::getConnection (host, port, login.c_str(), passwd.c_str());
  }
  catch (ConnectionException e)
  {
    cerr << e << endl;
    exit (1);
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
    exit (1);
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

  delete con;
  return 0;
}

void read_and_execute(istream &in, Connection *con ) {
  printMsg ("\n");
  
  string line;			// one line of a query
  string input;			// whole query (possibly multi line)

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
	  execute_query(con, input, NULL);
	}
      }	//while

  }
  catch (ConnectionException e)
  {
    cerr << e << endl;
    exit (1);
  }

};

/** result is result of the last query, possibly needed by $ans paramiter */
void execute_query(Connection *con, string &input, Result **result) {
  
  cerr << "<SBQLCli> Zapytanie: " << input << endl;
  
  try
    {
    
#ifdef ANS_PARAMETER_ADDITION_ACTIVE 
      cerr << "<SBQLCli> adding last result as a parameter $ans = " 
	   << **result << endl;
      Result* resultTmp;
      Statement* stmt;
      stmt   = con->parse(input.c_str());
      stmt->addParam("ans", *result);
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

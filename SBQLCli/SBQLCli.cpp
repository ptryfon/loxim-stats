
#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "../Driver/DriverManager.h"
#include "../Driver/Result.h"

using namespace std;
using namespace Driver;



void printMsg(string str) {
  if (isatty(0))
    cout << str;
}


int main(int argc, char *argv[]) {
  
  char* host = "localhost";
  int   port = 6543;
  
  /* args things */
  if (argc >= 2) {
    host = argv[1];
  }
  
  if (argc == 3) {
    port = atoi(argv[2]);
  }
  
  if (argc > 3) {
    cout << "usage: " << argv[0] << " host port " << endl;
    exit (1);
  }
  
  /* establishing connection */
  Connection* con;
  try {
    con = DriverManager::getConnection(host, port);
  } catch (ConnectionException e) {
    cerr << e << endl;
    exit(1);
  }
  
  printMsg("SBQLCli ver 1.0 \n");
  printMsg("Ctrl-d exits \n");

  string line;  // one line of a query
  string input; // whole query (possibly multi line)


  printMsg(" > ");

  try { 

    while (getline(cin, line)) {
      
      if (line.find(";",0) == string::npos) {

	input.append(line);
	printMsg(" \\ ");

      } else { // execute query, because  ";" found 
	
	input.append(line);
	cerr << "<SBQLCli> Zapytanie: " << input << endl;
	
	Result* result = NULL;
	
	try {
	
	  result = con->execute(input.c_str());
	  cout << *result << endl;
	
	} catch (ConnectionServerException e) { //parse error, or smth.
	  cerr << e << endl;
	}  
	
	input.clear();
	printMsg(" > ");
      }
      
    } //while
    
  } catch (ConnectionException e) {
    cerr << e << endl;
    exit(1);
  }
  
  
  /* sending abort on exit */
  cerr << endl << "<SBQLCli> auto abort on exit " << endl;
  Result* result = NULL; 
  try {
    result = con->execute("abort;");
    cout << *result << endl;
  } catch (ConnectionServerException e) {
    cerr << e << endl;
  }  
  

  return 0;		
}

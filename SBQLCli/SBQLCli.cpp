
#include <stdio.h>
#include <iostream>
#include <string>
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
  
  /*if (argc != 3) {
    cout << "SBQLCli host port" << endl;
    return 0;
    }*/
  
  if (argc >= 2) {
    host = argv[1];
  }
  
  if (argc == 3) {
    port = atoi(argv[2]);
  }
  
  
  Connection* con;
  try {
    con = DriverManager::getConnection(host, port);
  } catch (ConnectionException e) {
    cerr << e << endl;
    exit(1);
  }
  
  string input;
  string line;

  printMsg("SBQLCli ver 0.0.0.1 \n");
  printMsg(" > ");
  
  try { //Piotrek

    while (getline(cin, line)) {
      if (line.find(";",0) == string::npos) {
	input.append(line);
	printMsg(" \\ ");
      } else {
	input.append(line);
	cerr << "<SBQLCli> Zapytanie :" << input << endl;
	Result* result = con->execute(input.c_str());
	if (result != 0) { //Piotrek - mam nadzieje ze sie nie obrazisz :)
	                   //Czarek  - no jak¿e bym móg³ :)
	                   //Adam - ;-)
	  cout << *result;
	} else {cerr << "zapytanie nie powiodlo sie" << endl;}
	cout << endl;
	input.clear();
	printMsg(" > ");
      }
    }

  } catch (ConnectionException e) { //Piotrek
    cerr << e << endl;
    exit(1);
  }
  
  return 0;		
}

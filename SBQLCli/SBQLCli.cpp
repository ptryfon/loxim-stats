
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include "../Driver/DriverManager.h"
#include "../Driver/Result.h"

using namespace std;

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
  con = DriverManager::getConnection(host, port);
  
  
  char input[50];
  cout << "SBQLCli ver 0.0.0.0.1 \n";
  cout << " > ";
  while (cin.getline(input,50)) {
    Result* result = con->execute(input);
    if (result != 0) { //Piotrek - mam nadzieje ze sie nie obrazisz :)
    				   //Czarek  - no jak¿e bym móg³ :)
    	cout << *result;
    } else {cout << "zapytanie nie powiodlo sie" << endl;}
    cout << endl << " > ";
  };

  return 0;		

}

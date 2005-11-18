
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include "../Driver/DriverManager.h"
#include "../Driver/Result.h"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "SBQLCli host port" << endl;
		return 0;
	}
	
	char* host = argv[1];
	int   port = atoi(argv[2]);
	Connection* con;
	con = DriverManager::getConnection(host, port);

	
	char input[50];
	cout << "SBQLCli ver 0.0.0.0.0.1 \n";
	cout << " > ";
	while (cin.getline(input,50)) {
	  //Result* result = con->execute(input);
		//cout << result->toString();
		cout << "Dostalem wynik \n";
		cout << endl << " > ";
		//cout << input << "\n";
	};
	return 0;		
}

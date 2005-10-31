
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include "../Driver/DriverManager.h"


using namespace std;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "SBQLCli a b" << endl;
		return 0;
	}
	
	char* host = argv[1];
	int   port = atoi(argv[2]);
	Connection* con;
	try {
		con = DriverManager::getConnection(host, port);
	} catch (exception e) {
		cout << "Can't connect" << endl;
		cout << e.what();
	}
	
	char input[50];
	cout << "SBQLCli ver 0.0.0.0.0.1 \n";
	cout << " > ";
	while (cin.getline(input,50)) {
		ResultSet* result = con->execute(input);
		cout << result->toString();
		cout << endl << " > ";
		//cout << input << "\n";
	};
	return 0;		
}

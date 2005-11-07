#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 

using namespace std;

#include "Connection.h"
#include "ResultSet.h"

Connection::Connection(int socket)
{
	sock = socket;
}

Connection::~Connection()
{
}

ResultSet* Connection::execute(char* query){

      if (1 > send (sock, "", 1, 0))
      cerr << "blad przy wysylaniu do servera" << endl;
   else
       cerr << "zapytanie wyslano do serwera" << endl;
      
      char buf[4];
      int ile;
      
         ile = recv (sock, buf, 3, 0);
         if (ile < 0) cerr << "blad przy odbiorze" << endl;
         else cerr << "klient otrzymal wynik" << endl;

         return new ResultSet();
}

int Connection::disconnect() {
	return close(sock);
}

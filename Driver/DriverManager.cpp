#include <stdio.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 

#include "DriverManager.h"
#include "Connection.h"

using namespace std;

DriverManager::DriverManager()
{
}

DriverManager::~DriverManager()
{
}

Connection* DriverManager::getConnection(char* url, int port)
{
	//TODO throw some exception    

   // cout << "client, podaj port: ";
   // cin >> port;
    
    int sock;
    sock = socket( PF_INET, SOCK_STREAM, 0 );
    if (sock < 0) cerr << "problem z socketem" << endl;
  //  cout << "socket: " << sock << endl;


    sockaddr_in Addr;

Addr.sin_family = AF_INET; // PF_INET
//Addr.sin_addr.S_un.S_addr = htonl( ADDR_ANY );

Addr.sin_port=htons( port );
//////////////
struct hostent *hp;
char hostn[30];

hp = gethostbyname (url);
  if (hp == 0)
    {
      fprintf (stderr, "%s : unknown host\n", hostn);
      exit (2);
    }
  memcpy ((char *) &Addr.sin_addr, (char *) hp->h_addr, hp->h_length); 
///////////////

//Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

if (0 != connect( sock, (sockaddr*)&Addr, sizeof( Addr ) ))
   cerr << "blad w connect" << endl;
		
	return new Connection(sock);
}
/*
int main() {
	Connection* c = (new DriverManager())->getConnection("green",6543);
	c->disconnect();
	return 0;
}
*/

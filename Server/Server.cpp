#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include "Server.h"

using namespace std;

Server::Server(int newSock)
{
	Sock = newSock;
	memset(messgBuff, 0, sizeof(messgBuff));
}

Server::~Server()
{
}


//TODO - inicjacja obiektow
int Server::initializeAll() 
{
	return 0;
}

// Stary kod z CT.cpp - nie wglebialem sie
int Server::Run()
{
	char buf[4];
	int ile = 1;

	while (ile > 0)
   { 
		ile = recv (Sock, buf, 3, 0);
		if (ile > 0) {
                 //obsluzyc parser i query executor
                 
                 
                    if (1 > send (Sock, "", 1, 0)) {
                       cerr << "blad przy wysylaniu wyniku do klienta: " 
                       << endl;
                       ile = -1;
                       }
                    else
                       cerr << "wyslalem wynik do clienta" << endl;

                 
         }
         if (ile == 0) {
                 cerr << "klient sie rozlaczyl" << endl;
                 //rozlaczenie czy zerwanie                 
         }
         if (ile < 0) cerr << "blad przy odbiorze" << endl;
    }

	return 0;	
}		



#include "ConnectionThread.h"
#include <iostream>

using namespace std;

ConnectionThread::ConnectionThread(SOCKET nowysock)
{
	nowy = nowysock;
}

ConnectionThread::~ConnectionThread()
{
}

int ConnectionThread::run() 
{
	    char buf[4];
    int ile;
    
    do { 
         ile = recv (nowy, buf, 3, 0);
         
         if (ile > 0) {
                 cerr << "server dostal zapytanie" << endl;
                 //obsluzyc parser i query executor
                 
                 
                    if (1 > send (nowy, "", 1, 0)) {
                       cerr << "blad przy wysylaniu wyniku do klienta: " 
                       << WSAGetLastError( ) << endl;
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
         while (ile > 0);

	return 0;	
}
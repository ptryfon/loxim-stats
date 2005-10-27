#include <cstdlib>
#include <iostream>
//#include <sys/socket.h>
#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

using namespace std;

int Server::createSocket(int port) {
	
	cout << "server, podaj port: " << endl;
    cin >> port;
    
    WSADATA WSD;
    if (0 != WSAStartup( MAKEWORD( 2 , 0 ) , &WSD))
       cout << "Byl blad: " << WSAGetLastError( ) << endl;

    SOCKET sock;
    sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if (sock < 0) cerr << "problem z socketem" << endl;
    cout << "socket: " << sock << " inv_sock: " << INVALID_SOCKET << endl;
    
    char nazwa[30];
    int dl_nazwy = 29;
    
    if (0 != gethostname( nazwa, dl_nazwy ))
       cerr << "nie mam nazwy hosta" << endl;
    cout <<  nazwa << endl;
    
    sockaddr_in Addr;

	Addr.sin_family = AF_INET; // PF_INET
	Addr.sin_port = htons( port );
	Addr.sin_addr.S_un.S_addr = htonl( ADDR_ANY );
	// zerujemy resztê struktury
	memset( Addr.sin_zero , 0, 8 );
    
    if (0 > bind (sock, (sockaddr*)&Addr, sizeof( sockaddr ) ))
       cout << "Byl blad przy bindowaniu: " << WSAGetLastError( ) << endl;

    return 0;
}

int Server::listenOnSock(SOCKET sock) 
{
    if (0 != listen( sock, 1 ))
       cerr << "blad w listen" << WSAGetLastError( ) << endl;
    
    int  s;
	SOCKET nowy;

	while (true) do
	{
    	if (INVALID_SOCKET == (nowy = accept( sock, (sockaddr*) 0, (int *)0 )))
        {
        	cerr << "blad w accept " << WSAGetLastError( ) << endl;	
        } else 
		{
			createThread(nowy);	
		}
	}

    return 0;
}

int Server::createThread(SOCKET nowy) 
{
	    	
}

int Server::run()
{
	createSocket(7777);
		
}
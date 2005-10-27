#include "DriverManager.h"
#include <iostream>

using namespace std;

DriverManager::DriverManager()
{
}

DriverManager::~DriverManager()
{
}

static DriverManager::getConnection(char* url, int port)
{
	//TODO throw some exception    
    int port;
    cout << "client, podaj port: ";
    cin >> port;
    
    WSADATA WSD;
    if (0 != WSAStartup( MAKEWORD( 2 , 0 ) , &WSD))
       cout << "Byl blad: " << WSAGetLastError( ) << endl;

    SOCKET sock;
    sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if (sock < 0) cerr << "problem z socketem" << endl;
    cout << "socket: " << sock << " inv_sock: " << INVALID_SOCKET << endl;


    sockaddr_in Addr;

Addr.sin_family = AF_INET; // PF_INET
//Addr.sin_addr.S_un.S_addr = htonl( ADDR_ANY );

Addr.sin_port=htons( port );
Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

if (0 != connect( sock, (sockaddr*)&Addr, sizeof( Addr ) ))
   cerr << "blad w connect, blad nr: " << WSAGetLastError( ) << endl;
		
	return new Connection(sock)
}

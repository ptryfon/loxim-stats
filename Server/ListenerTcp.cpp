#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <arpa/inet.h>

#include "Listener.h"

using namespace std;

/*
 * parametr socket musi byc przekazany przez referencje
 */

int Listener::CreateSocket(int port, int* created_socket) {
	
    int sock;
    cerr << "entering CreateSocket" << endl;
    sock = socket( PF_INET, SOCK_STREAM, 0 );
    if (sock < 0) {
    	cerr << "problem z socketem" << endl;
    	return 1;
    }
    
    char nazwa[30];
    int dl_nazwy = 29;
    
    if (0 != gethostname( nazwa, dl_nazwy )) {
       cerr << "nie mam nazwy hosta" << endl;
       return 1;
    }
    cout <<  nazwa << endl;
    
    sockaddr_in Addr;

	Addr.sin_family = AF_INET; // PF_INET
	Addr.sin_port = htons( port );
	Addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	// zerujemy reszt� struktury
	memset( Addr.sin_zero , 0, 8 );
    
    if (0 > bind (sock, (sockaddr*)&Addr, sizeof( sockaddr ) )) {
       cerr << "blad przy bindowaniu" << endl;
       return 1;
    }
    
   //string s(Addr.sin_addr);
   
   //zeby wiedziec jaki adres ma server
   struct hostent *hp;
   
   hp = gethostbyname (nazwa);
  if (hp == 0)
    {
      fprintf (stderr, "%s : unknown host\n", nazwa);
      exit (2);
    }
  memcpy ((char *) &Addr.sin_addr, (char *) hp->h_addr, hp->h_length); 
   
   printf ("%s \n", inet_ntoa(Addr.sin_addr));
 
	*created_socket = sock;

	cerr << "End CreateSocket" << endl;
    return 0;
}

/*
 * parametr newSocket musi byc przekazany przez referencje
 * w wyniku bedzie wskazywal na nowy socket do ktorego przylaczyl sie nowy klient
 */

int Listener::ListenOnSocket(int sock, int* newSocket) 
{

	printf("ListenerTcp: Listening on socket number %d \n", sock);
    if (0 != listen( sock, 1 )) {
       cerr << "blad w listen" << endl;
       return 1;
    }
    
    int nowy;

    	if ((nowy = accept( sock, (sockaddr*) 0, 0 )) == -1)
        {
        	cerr << "blad w accept " << endl;	
        	return 1;
        } else
        { 
		printf("ListenerTcp: accepted \n");
			*newSocket = nowy;
			    return 0;
	}
}

int Listener::CloseSocket(int sock) {
	if (0 == close(sock)) return 0;
	else {
		cerr << "blad przy zamykaniu gniazda" << endl;
		return 1;
	}
}

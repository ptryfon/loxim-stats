#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <arpa/inet.h>
#include <errno.h>

#include "../Errors/Errors.h"
#include "Listener.h"

using namespace std;

/*
 * parametr socket musi byc przekazany przez referencje
 */

int Listener::CreateSocket(int port, int* created_socket, bool reuse) {
	
    int sock;
  //  *lCons << "entering CreateSocket";
    sock = socket( PF_INET, SOCK_STREAM, 0 );
    if (sock == -1) {
    	*lCons << "problem z socketem";
    	return errno | ErrTCPProto;
    }
    
  //  lCons->printf("<ListenerTcp::createSocket> utworzone gniazdo nasluchu: %d\n", sock);
  
     
   if (reuse)
    {
      int on = 1;
      if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0)
        {
          //setsockopt failed but maybe server can run without it -> don't shutdown yet
           // perror("setsockopt(SO_REUSEADDR) failed");
           lCons->printf("blad setsockopt: %s\n", strerror(errno));
        }
    }
    
    
    char nazwa[30];
    int dl_nazwy = 29;
    
    if (0 != gethostname( nazwa, dl_nazwy )) {
       *lCons << "nie mam nazwy hosta";
       return errno | ErrTCPProto;
    }
    *lCons << nazwa;
    
    sockaddr_in Addr;

	Addr.sin_family = AF_INET; // PF_INET
	Addr.sin_port = htons( port );
	Addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	// zerujemy resztê struktury
	memset( Addr.sin_zero , 0, 8 );
    
    if (0 != bind (sock, (sockaddr*)&Addr, sizeof( sockaddr ) )) {
       lCons->printf("blad przy bindowaniu: %s\n", strerror(errno));
       return errno | ErrTCPProto;
    }
    //TODO reszte tej funkcji mozna wyrzucic, jest tylko do celow diagnostycznych
   //string s(Addr.sin_addr);
   
   //zeby wiedziec jaki adres ma server
   struct hostent *hp;
   
   hp = gethostbyname (nazwa);
  if (hp == 0)
    {
      lCons->printf("%s : unknown host\n", nazwa);
      return ENoHost | ErrTCPProto;
    }
  memcpy ((char *) &Addr.sin_addr, (char *) hp->h_addr, hp->h_length); 
   
   lCons->printf ("%s \n", inet_ntoa(Addr.sin_addr));
 
	*created_socket = sock;

//	*lCons << "End CreateSocket";
    return 0;
}

/*
 * parametr newSocket musi byc przekazany przez referencje
 * w wyniku bedzie wskazywal na nowy socket do ktorego przylaczyl sie nowy klient
 */

int Listener::ListenOnSocket(int sock, int* newSocket, int queueLength) 
{

//	lCons->printf("ListenerTcp: Listening on socket number %d \n", sock);
    if (0 != listen( sock, queueLength )) {
       *lCons << "blad w listen";
       return errno | ErrTCPProto;
    }
    
    int nowy;

    	if ((nowy = accept( sock, (sockaddr*) 0, 0 )) == -1)
        {
        	*lCons << "blad w accept";	
        	return 1;
        } else
        { 
	//	lCons->printf("<ListenerTcp> accepted, nowe gniazdo watku: %d\n", nowy);
			*newSocket = nowy;
			    return 0;
	}
}

int Listener::CloseSocket(int sock) {
	if (0 == close(sock)) {
	//lCons->printf("gniazdo: %d zamkniete pomyslnie\n", sock);
	return 0;
	}
	else {
		*lCons << "blad przy zamykaniu gniazda";
		return errno | ErrTCPProto;
	}
}


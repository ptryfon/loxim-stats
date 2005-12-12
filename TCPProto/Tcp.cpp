#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <errno.h>

#include "Tcp.h"
#include "../Errors/Errors.h"

namespace TCPProto {

using namespace std;

int bufferSend(char* buf, int buf_size, int sock) {
	//printf("ServerTcp: sending data on socket %d \n", sock);
	
	char* lengthBuf;
	int lengthBufSize = 4;
	int ile;
	ile = htonl(buf_size);
	lengthBuf = (char*)(& ile);

   	cout << "ile bajtow wysylam: " << buf_size << endl;
 //  	cout << "co wysylam: " << buf << endl;
	// lengthBuf = ...buf_size
	while (lengthBufSize > 0) {
		if (-1 ==  (ile = send(sock, lengthBuf, lengthBufSize, 0))) {
			cerr << "blad wysylania" << endl;
			return errno | ErrTCPProto;
		}
		lengthBufSize -= ile;
		lengthBuf += ile;	
	}
	
	while (buf_size > 0) {
	if (-1 == (ile = send(sock, buf, buf_size, 0))) {
		cerr << "blad wysylania" << endl;
		return errno | ErrTCPProto;
	}
	
		buf_size -= ile;
		buf += ile;
	}
	
//	printf("ServerTcp: Send-end \n");
	return 0;
}

/* 
 * parametr musi byc przekazany przez referencje
 * 
 * wynik:
 * 0 - wszystko ok, wtedy *receiveDataSize = ile bejtow wczytano do bufora
 * wpp. kod bledu
 */


int bufferReceive (char** buffer, int* receiveDataSize, int sock) {
	//printf("ServerTcp: receiving on socket %d... \n", sock);
		
         int ile;
         int msgSize = 0;
         int rest = 4;         
         char* messgBuff;
         char* messgBuffBeg;
         
         char lengthBuffTable[3];
         char* lengthBuff = lengthBuffTable;
         
          while (rest > 0) {
         	ile = recv (sock, lengthBuff, rest, 0);
         	
         	if (ile < 0) return errno | ErrTCPProto; //error
         	if (ile == 0) return ECONNABORTED | ErrTCPProto; //disconnect
         	
         	rest -= ile;
         	lengthBuff += ile;
         }
         
         if (rest == 0) {
         		// mamy juz cala liczbe okreslajaca dlugosc
         		msgSize = ntohl(*((unsigned long int*) lengthBuffTable));
         		// msgSize = ...;
         	cout << "ile bajtow dostane: " << msgSize << endl;	
         	}
         
         messgBuffBeg = messgBuff = (char*) malloc(msgSize);
         
         if (messgBuff == NULL) return errno | ErrTCPProto;
         
         // parameterPtr = messgBuff;
         
         rest = msgSize;
         
        while (rest > 0) {
         	
        ile = recv (sock, messgBuff, rest, 0);
         
        if (ile < 0) return errno | ErrTCPProto;
        if (ile == 0) return ECONNABORTED | ErrTCPProto;
        
        rest -= ile;
        messgBuff += ile;
        
        }
        
     //  	cout << "a oto tresc: " << messgBuffBeg << endl;
        
        *buffer = messgBuffBeg; //zwrocic wynik
        
	 //	printf("ServerTcp: received some data --> size = %d \n", ile);
         	 *receiveDataSize = msgSize;
                 return 0;               
}

} //namespace

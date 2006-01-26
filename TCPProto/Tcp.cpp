#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <errno.h>

#include "Tcp.h"
#include "../Errors/Errors.h"

namespace TCPProto {

using namespace std;

int bufferSend(const char* buf, int buf_size, int sock) {
	//printf("ServerTcp: sending data on socket %d \n", sock);
	
	//TODO zdecydowac czy beda signed czy unsigned
	
	if (buf_size <= 0) return EINVAL | ErrTCPProto; //message size has to be positive
	
	char* lengthBuf;
	int lengthBufSize = 4; //int = 4 bytes
	int ile;
	int msg_size = htonl(buf_size);
	lengthBuf = (char*)(& msg_size); //contains the size of the message

   	cerr << "<Tcp::bufferSend()> ile bajtow wysylam: " << buf_size << endl;
   	
	while (lengthBufSize > 0) { //if 0, the size was sent
		if (-1 ==  (ile = send(sock, lengthBuf, lengthBufSize, MSG_NOSIGNAL))) {
			cerr << "blad wysylania 1: " << strerror(errno) << endl;
			return errno | ErrTCPProto;
		}
		lengthBufSize -= ile;
		lengthBuf += ile;	
	}
	cerr << "<TCP::bufferSedn> znacznik przesylania, ile: " << ile << endl;
	while (buf_size > 0) {
	if (-1 == (ile = send(sock, buf, buf_size, MSG_NOSIGNAL))) {
		cerr << "blad wysylania 2" << strerror(errno) << endl;
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
 * 0 - wszystko ok, wtedy 0 < *receiveDataSize = ile bajtow wczytano do nowo utworzonego bufora
 * 0 - nadawca sie rozlaczyl, wtedy *buffer = null, *receiveDataSize = 0
 * wpp. kod bledu
 */


int bufferReceive (char** buffer, int* receiveDataSize, int sock) {
		
         int ile;
         int msgSize = 0;
         int rest = 4;         
         char* messgBuff; //position in the message buffer
         char* messgBuffBeg; //begining of the message buffer
                  
         // in case of error
         *receiveDataSize = 0;
         *buffer = NULL;
         
         char lengthBuffTable[3];
         char* lengthBuff = lengthBuffTable;

          while (rest > 0) {
	        cerr << "<tcp::receive> zaczynam recv" << endl;
         	ile = recv (sock, lengthBuff, rest, 0);
         	cerr << "<tcp::receive> recv zakonczone" << endl;
         	if (ile < 0) {
		cerr << "<tcp::receive> blad czytania tcp: " <<
		strerror (errno) << endl;
		return errno | ErrTCPProto;
		}
         	if (ile == 0) {
		cerr << "<tcp::receive> zamknieto gniazdo podczas czytania" << endl;
		return 0; //disconnect
		}
         cerr << "<tcp::receive> po czytaniu w petli, ile = " << ile << endl; 
         	rest -= ile;
         	lengthBuff += ile;
         }
         cerr << "<tcp::receive> po czytaniu poza petla" << endl;
         if (rest != 0) {
         	//TODO wyrzucic to
         	cerr << "<Tcp::bufferReceive> to sie nie moze zdazyc!!!" << endl;
         	return 1;	
         } 
         		// mamy juz cala liczbe okreslajaca dlugosc
         	msgSize = ntohl(*((unsigned long int*) lengthBuffTable));
         	cerr << "<Tcp::bufferSend()> ile bajtow dostane: " << msgSize << endl;	
         	
         
         messgBuffBeg = messgBuff = (char*) malloc(msgSize + 1); //+1 protection from reading outside the buffer
         
         if (messgBuff == NULL) return errno | ErrTCPProto;
         messgBuff[msgSize] = 0; //now string reading never leads outside the buffer
         
         rest = msgSize;
         
        while (rest > 0) {
        	ile = recv (sock, messgBuff, rest, 0);
        	
        	if (ile < 0) {
        		int error = errno;
			cerr << "<tcp::receive> blad czytania tcp: " <<
		strerror (errno) << endl;
        		free(messgBuffBeg);
        		return error | ErrTCPProto;
        	}
        	if (ile == 0) {
				cerr << "<tcp::receive> zamknieto gniazdo podczas czytania" << endl;
        		free(messgBuffBeg);
        		return 0; //disconnect
        	}
        	
        	rest -= ile;
        	messgBuff += ile;
        }
        
        //return results
        *buffer = messgBuffBeg;
        *receiveDataSize = msgSize;
        return 0;               
}

} //namespace

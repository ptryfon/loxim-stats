#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Tcp.h"
#include "../Errors/Errors.h"
#include "Package.h"

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

   	//cerr << "<Tcp::bufferSend()> ile bajtow wysylam: " << buf_size << endl;

	while (lengthBufSize > 0) { //if 0, the size was sent
		if (-1 ==  (ile = send(sock, lengthBuf, lengthBufSize, MSG_NOSIGNAL))) {
		//	cerr << "blad wysylania 1: " << strerror(errno) << endl;
			return errno | ErrTCPProto;
		}
		lengthBufSize -= ile;
		lengthBuf += ile;
	}
//	cerr << "<TCP::bufferSedn> znacznik przesylania, ile: " << ile << endl;
	while (buf_size > 0) {
	if (-1 == (ile = send(sock, buf, buf_size, MSG_NOSIGNAL))) {
	//	cerr << "blad wysylania 2" << strerror(errno) << endl;
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
	  //      cerr << "<tcp::receive> zaczynam recv" << endl;
         	ile = recv (sock, lengthBuff, rest, 0);
       //  	cerr << "<tcp::receive> recv zakonczone" << endl;
         	if (ile < 0) {
	//	cerr << "<tcp::receive> blad czytania tcp: " <<
	//	strerror (errno) << endl;
		return errno | ErrTCPProto;
		}
         	if (ile == 0) {
	//	cerr << "<tcp::receive> zamknieto gniazdo podczas czytania" << endl;
		return 0; //disconnect
		}
     //    cerr << "<tcp::receive> po czytaniu w petli, ile = " << ile << endl;
         	rest -= ile;
         	lengthBuff += ile;
         }
         		// mamy juz cala liczbe okreslajaca dlugosc
         	msgSize = ntohl(*((unsigned long int*) lengthBuffTable));
         	//cerr << "<Tcp::bufferReceive()> ile bajtow dostane: " << msgSize << endl;


         messgBuffBeg = messgBuff = (char*) malloc(msgSize + 1); //+1 protection from reading outside the buffer

         if (messgBuff == NULL) return errno | ErrTCPProto;
         messgBuff[msgSize] = 0; //now string reading never leads outside the buffer

         rest = msgSize;

        while (rest > 0) {
        	ile = recv (sock, messgBuff, rest, 0);

        	if (ile < 0) {
        		int error = errno;
		//	cerr << "<tcp::receive> blad czytania tcp: " <<
	//	strerror (errno) << endl;
        		free(messgBuffBeg);
        		return error | ErrTCPProto;
        	}
        	if (ile == 0) {
		//		cerr << "<tcp::receive> zamknieto gniazdo podczas czytania" << endl;
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

int packageReceive(Package** package, int sock) {

	char* ptr;
	int size, error;
	error = bufferReceive(&ptr, &size, sock);
	if (error != 0) {
		//cerr << "buffer receive failed\n";
		return error;
	}

	if (size < 1) {
		//cerr << "rozmiar < 1, rowny: "<<size <<"\n";
		return -2; //TODO
	}
	switch (ptr[0]) {
		case Package::SIMPLEQUERY:
			*package = new SimpleQueryPackage();
			break;
		case Package::PARAMQUERY:
			*package = new ParamQueryPackage();
			break;
		case Package::STATEMENT:
			*package = new StatementPackage();
			break;
		case Package::SIMPLERESULT:
			*package = new SimpleResultPackage();
			break;
		default:
			//TODO error
			return -2;
		//	cerr << "bledny typ package::type\n";
	}

	if (0 != (error = (*package)->deserialize(ptr, size)))
	{
		return error;
	}
	return 0;
}

int packageSend(Package* package, int sock) {
	int error;
	char* buffer;
	int   size;
	error = package->serialize(&buffer, &size); // t allocs memory...
	if (error != 0) {
		return error;
	}
	//cerr << "po serializacji: |" << buffer << "| rozmiar: "<<size << endl;
	return bufferSend(buffer, size, sock);
}


int getConnection(char* url, int port, int* newsock) {
	int sock;
	sock = socket( PF_INET, SOCK_STREAM, 0 );
	if (sock < 0)
		return errno | ErrTCPProto;

	//cerr << "problem z socketem" << endl;
	//  cout << "socket: " << sock << endl;


	sockaddr_in Addr;

	Addr.sin_family = AF_INET; // PF_INET
	//Addr.sin_addr.S_un.S_addr = htonl( ADDR_ANY );

	Addr.sin_port=htons( port );
	//////////////
	struct hostent *hp;
	//char hostn[30];

	hp = gethostbyname (url);
	if (hp == 0)
	{
		return ENoHost | ErrTCPProto;
	//fprintf (stderr, "%s : unknown host\n", hostn);
	//exit (2);
	}


	memcpy ((char *) &Addr.sin_addr, (char *) hp->h_addr, hp->h_length);
	///////////////

	//Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	if (0 != connect( sock, (sockaddr*)&Addr, sizeof( Addr ) ))
		return errno | ErrTCPProto;
	//cerr << "blad w connect" << endl;

	*newsock = sock;
	return 0;
}

} //namespace

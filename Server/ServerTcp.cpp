#include <stdio.h>
#include <sys/socket.h>
#include <iostream>
#include "Server.h"

using namespace std;

/* klasa obslugujaca konkretne polaczenie musi zawierac te dwa pola
 *

private:
	char buffer[60];
	int socket; //tutaj w konstruktorze trzeba przypisac gniazdo

*/

/* 
 * parametr musi byc przekazany przez referencje
 * 
 * wynik:
 * 0 - wszystko ok, wtedy *receiveDataSize = ile bejtow wczytano do bufora
 * wpp. kod bledu
 */

int Server::Receive(int* receiveDataSize) { 
         int ile = recv (Sock, messgBuff, 3, 0);
         
         if (ile >= 0) {
         	 
         	 *receiveDataSize = ile;
                 return 0;               
         }
		else {
         cerr << "blad przy czytaniu z gniazda" << endl;
         return 1;
		}
}

int Server::Send(char* buf, int buf_size) {
	if (0 > send(Sock, buf, buf_size, 0)) {
		cerr << "blad wysylania" << endl;
		return 1;
	}
	
	return 0;
}

int Server::Disconnect(){
	return close(Sock);
}


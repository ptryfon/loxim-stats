#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <string.h>

using namespace std;

#include "Connection.h"
#include "Result.h"

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
         	
         	if (ile < 0) return 1; //error
         	if (ile == 0) return 2; //disconnect
         	
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
         
         if (messgBuff == 0) return 1; //TODO sprawdz czy blad
         
         // parameterPtr = messgBuff;
         
         rest = msgSize;
         
        while (rest > 0) {
         	
        ile = recv (sock, messgBuff, rest, 0);
         
        if (ile < 0) return 1;
        if (ile == 0) return 2;
        
        rest -= ile;
        messgBuff += ile;
        
        }
        
       	cout << "a oto tresc: " << messgBuffBeg << endl;
        
        *buffer = messgBuffBeg; //zwrocic wynik
        
	 //	printf("ServerTcp: received some data --> size = %d \n", ile);
         	 *receiveDataSize = msgSize;
                 return 0;               
}


int bufferSend(char* buf, int buf_size, int sock) {
	//printf("ServerTcp: sending data on socket %d \n", sock);
	
	char* lengthBuf;
	int lengthBufSize = 4;
	int ile;
	ile = htonl(buf_size);
	lengthBuf = (char*)(& ile);

   	cout << "ile bajtow wysylam: " << buf_size << endl;
   	cout << "co wysylam: " << buf << endl;
	// lengthBuf = ...buf_size
	while (lengthBufSize > 0) {
		if (0 >= (ile = send(sock, lengthBuf, lengthBufSize, 0))) {
		cerr << "blad wysylania" << endl;
		return 1;
		}
		lengthBufSize -= ile;
		lengthBuf += ile;	
	}
	
	while (buf_size > 0) {
	if (0 > (ile = send(sock, buf, buf_size, 0))) {
		cerr << "blad wysylania" << endl;
		return 1;
	}
	
		buf_size -= ile;
		buf += ile;
	}
	
//	printf("ServerTcp: Send-end \n");
	return 0;
}

Connection::Connection(int socket)
{
	sock = socket;
}

Connection::~Connection()
{
}

int Connection::stringCopy(char* &newBuffer) { // od bufferBegin
		//TODO dodac sprawdzanie zakresu, przepelnienia bufora!!!
		int len = strlen(bufferBegin)+1; //including NULL
		newBuffer = (char*)malloc (len);
		strcpy (newBuffer, bufferBegin);
		bufferBegin += len;
		return 0;
}

int Connection::getULong(unsigned long &val) {
	if ((bufferBegin + sizeof (long)) <= bufferEnd) {
		val = ntohl(*((unsigned long*) bufferBegin));
		bufferBegin += sizeof (long); //skip the number of elements (long)
		return 0;
	} else {
		cout << "<Connetion::getULong> proba czytania poza odebranym buforem" << endl;
		return 1;
	}
}

int Connection::deserialize(Result** rs) {
	int error;
//	bufferBegin++;
	ResultBag *brs;
	Result* result;	
	char* id;
	unsigned long number;
	unsigned long i;
		
	switch (*(bufferBegin++)) {
		case Result::BAG:
			cout << "<Connection::deserialize> tworze obiekt BAG\n";
//			bufferBegin++; //skip first byte
			if (0 != getULong(number)) return 1; //by reference 

			brs = new ResultBag(number);
			
			for (i = 1; i <= number; i++) {
				if(0 != (error = deserialize(&result))) {
					//TODO w razie bledu zniszczyc brs jak juz destruktor bedzie dzialac
					*rs = NULL;
					return error; //TODO poprawic sprawdzanie bledu
				}
				brs->add(result);
			}
			*rs = brs;
			return 0;
		
		case Result::REFERENCE:
			cout << "<Connection::deserialize> tworze obiekt REFERENCE\n";
	//		bufferBegin++;
			stringCopy(id); //by reference
			*rs = (Result*) new ResultReference(id);
			//TODO poprawic przesuniecie wskaznika
			return 0;
		
		case Result::VOID:
		cout << "<Connection::deserialize> tworze obiekt VOID\n";
		//	bufferBegin++;
			*rs = new ResultVoid();
			return 0; 
		
		case Result::STRING:
			cout << "<Connection::deserialize> tworze obiekt STRING\n";
//			bufferBegin++;
			stringCopy(id); // by reference
			*rs = (Result*) new ResultString(string (id));
			//co z id czy string je zabiera czy niszczy
			return 0;
		
		case Result::ERROR:
			cout << "<Connection::deserialize> tworze obiekt ERROR\n";
			if (0 != getULong(number)) return 1; //by reference 
			*rs = new ResultError(number);
			return 0;
			
		case Result::SEQUENCE:
		
		case Result::STRUCT:
		
		case Result::INT:
		
		case Result::BOOL:
		
		case Result::DOUBLE:
		
		case Result::BINDER:
		
		default:
			cout << "<Connection::deserialize> obiekt nieznany lub jeszcze niezaimplementowany" << endl;
			*rs = NULL;
			return 1;//w przyszlosci bedzie tu nr bledu
	}
}

Result* Connection::execute(char* query){
	
	int error;

      error = bufferSend(query, strlen(query)+1, sock);
      if (0 != error) {
      	return new ResultError("sending error");
      }
      char* ptr;
      int ile;
      error = bufferReceive(&ptr, &ile, sock); //create buffer and set ptr to point at it
      if (error != 0) {
      	if (ptr != NULL) free(ptr);
      	return new ResultError("receiving error");
      }
      cout << "<Connection::execute> driver przyszlo bajtow: " << ile << " a tresc to: " << ptr << endl;
      cout << "<Connection::execute> driver odebral" << endl;
      bufferBegin = ptr;
      bufferEnd = ptr+ile;
      
      Result* rs;
      int error = deserialize(&rs);
      cout << "<Connection::execute> wynik deserializacji: " << error << endl;
      free(ptr); //free buffer created by bufferReceive()
      if (0 != error)
      	return new ResultError("blad protokolu");
      cout << "<Connection::execute> obiekt Result stworzony -> procedura zakonczona sukcesem" << endl;
      return rs;
}

int Connection::disconnect() {
	return close(sock);
}

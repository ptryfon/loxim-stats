#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <string.h>

#include "../TCPProto/Tcp.h"
#include "Connection.h"
#include "Result.h"

using namespace std;
using namespace TCPProto;


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
    //  cout << "<Connection::execute> wysylanie zakonczone" << endl;
      char* ptr = NULL;
      int ile;
      error = bufferReceive(&ptr, &ile, sock); //create buffer and set ptr to point at it
    //  cout << "<Connection::execute> odbieranie zakonczone" << endl;
      if (error != 0) {
    //  	if (ptr != NULL) {
    //  		cout << "<Connection::execute> zwalnianie wskaznika" << endl;
      		free(ptr); // if ptr == NULL nothing happens
     // 		cout << "<Connection::execute> wskaznik zwolniony" << endl;
     // 	}
      	return new ResultError("receiving error");
      }
      cout << "<Connection::execute> driver przyszlo bajtow: " << ile << " a tresc to: " << ptr << endl;
      cout << "<Connection::execute> driver odebral" << endl;
      bufferBegin = ptr;
      bufferEnd = ptr+ile;
      
      Result* rs;
      error = deserialize(&rs);
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

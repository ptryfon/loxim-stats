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
		
		if (bufferBegin + len > bufferEnd) {
			cout << "<Serialize::StringCopy> proba czytania poza buforem" << endl;
			throw ConnectionException("protocol corrupt");
		}
		
		newBuffer = (char*)malloc (len);
		strcpy (newBuffer, bufferBegin);
		//TODO sprawdzac czy sie powiodlo
		bufferBegin += len;
		return 0;
}

unsigned long Connection::getULong(unsigned long &val) {
	
	char* tmpPtr = bufferBegin + sizeof(long);
	
	if (tmpPtr > bufferEnd) {
		cout << "<Connetion::getULong> proba czytania poza odebranym buforem" << endl;
		throw ConnectionException("protocol corrupt");
	}
	
		val = ntohl(*((unsigned long*) bufferBegin));
		bufferBegin = tmpPtr; //skip the number of elements (long)
		return val;
}


Result* Connection::grabElements(ResultCollection* col) {
			
			unsigned long number;
			getULong(number);
//			col->setSize(getULong(number)); //by reference 			
			for (int i = 1; i <= number; i++) {
					//TODO w razie bledu zniszczyc
				col->add(deserialize());
			}
			return col;	
}


Result* Connection::deserialize() {
//	int error;
//	ResultBag *brs;
//	Result* result;	
	char* id;
	unsigned long number;
//	unsigned long i;
	unsigned long* lptr;
	double db;
	char df;
		
	switch (*(bufferBegin++)) {
		case Result::BAG:
			cout << "<Connection::deserialize> tworze obiekt BAG\n";
			return grabElements(new ResultBag());
			
		case Result::SEQUENCE:
			cout << "<Connection::deserialize> tworze obiekt SEQUENCE\n";
			return grabElements(new ResultSequence());
		
		case Result::STRUCT:
			cout << "<Connection::deserialize> tworze obiekt STRUCT\n";
			return grabElements(new ResultStruct());
		
		
		case Result::REFERENCE:
			cout << "<Connection::deserialize> tworze obiekt REFERENCE\n";
	//		bufferBegin++;
			stringCopy(id); //by reference
			return (Result*) new ResultReference(string (id));
			//TODO poprawic przesuniecie wskaznika
		
		case Result::VOID:
		cout << "<Connection::deserialize> tworze obiekt VOID\n";
			return new ResultVoid(); 
		
		case Result::STRING:
			cout << "<Connection::deserialize> tworze obiekt STRING\n";
			stringCopy(id); // by reference
			return (Result*) new ResultString(string (id));
		
		case Result::ERROR:
			cout << "<Connection::deserialize> tworze obiekt ERROR\n";
			getULong(number); //by reference 
			return new ResultError(number);
			
		
		case Result::INT:
					cout << "<Connection::deserialize> tworze obiekt INT\n";
			getULong(number); //by reference
			return new ResultInt((int) number);
		
		case Result::BOOLTRUE:
					cout << "<Connection::deserialize> tworze obiekt BOOL (true)\n";
			return new ResultBool(true);
		
		case Result::BOOLFALSE:
					cout << "<Connection::deserialize> tworze obiekt BOOL (false)\n";
			return new ResultBool(false);
		
		case Result::DOUBLE:
					cout << "<Connection::deserialize> tworze obiekt DOUBLE\n";
			lptr = (unsigned long*) &db;
			getULong(*lptr); //higher word
			getULong(*(lptr+1)); //lower word
			return new ResultDouble(db);
		
		case Result::BINDER:
	/*		cout << "<Connection::deserialize> tworze obiekt BINDER\n";
			getULong(number);
			return new ResultBinder(number, deserialize());
		*/
		default:
			df = *(bufferBegin-1);
			cout << "<Connection::deserialize> obiekt nieznany, nr: " << (int) df << endl;
			throw ConnectionException("protocol corrupt");
	} // switch
} // deserialize

Result* Connection::execute(char* query) throw (ConnectionException) {
	
	int error;

      error = bufferSend(query, strlen(query)+1, sock);
      if (0 != error) {
      	throw ConnectionException("sending error");
    //  	return new ResultError("sending error");
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
     	throw ConnectionException("receiving error");
      //	return new ResultError("receiving error");
      }
      cout << "<Connection::execute> driver przyszlo bajtow: " << ile << endl;
      bufferBegin = ptr;
      bufferEnd = ptr+ile;
      
      Result* rs = deserialize();
      free(ptr); //free buffer created by bufferReceive()
      
      cout << "<Connection::execute> obiekt Result stworzony -> procedura zakonczona sukcesem" << endl;
      return rs;
}

int Connection::disconnect() {
	return close(sock);
}


ostream& operator<<(ostream& os, ConnectionException& e) {
	e.toStream(os);
	return os;
}

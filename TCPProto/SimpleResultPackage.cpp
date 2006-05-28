#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <string.h>
#include <memory.h>
#include <errno.h>
#include "../Errors/Errors.h"
#include "unistd.h"

//#include "Tcp.h"
//#include "../Driver/Connection.h"
#include "../Driver/Result.h"
#include "Package.h"

namespace TCPProto {

using namespace std;
//using namespace TCPProto;

	int SimpleResultPackage::serialize(char** buffer, int* size) {
		return -1;
	}

	void SimpleResultPackage::setResult(Result *r) {
		tmpRes = r;
	}
	
	Result * SimpleResultPackage::getResult() {
		return tmpRes;
	}

	int SimpleResultPackage::stringCopy(char* &newBuffer) { // od bufferBegin
		int len = strlen(bufferBegin)+1; //including NULL
		
		if (bufferBegin + len > bufferEnd) {
			return -2; //TODO error czytanie poza buforem
		}
		
		newBuffer = (char*)malloc (len);
		
		if (NULL == newBuffer) {
			return (errno | ErrDriver);
		}
		strcpy (newBuffer, bufferBegin); // always succeeds
		bufferBegin += len;
		return 0;
}

int SimpleResultPackage::getULong(unsigned long &val) {
	
	char* tmpPtr = bufferBegin + sizeof(long);
	
	if (tmpPtr > bufferEnd) {
		return -2; //TODO proba czytania poza buforem
	}
	
		val = ntohl(*((unsigned long*) bufferBegin));
		bufferBegin = tmpPtr; //skip the number of elements (long)
		return 0;
}


int SimpleResultPackage::grabElements(ResultCollection* col) {
			
			unsigned long number;
			int error;
			if (0 != (error = getULong(number))) return error;
			Result* r;
//			col->setSize(getULong(number)); //by reference
			for (unsigned long i = 1; i <= number; i++) {
				if (0 != (error = dataDeserialize(&r))) return error;
				col->add(r);
			}
			return 0;
}

 int SimpleResultPackage::deserialize(char* buffer, int size) {
 	if (buffer[0] != getType()) return -1; //TODO
	bufferBegin = buffer+1;
	bufferEnd = buffer+size;
	int error;
	Result* r;
	error = dataDeserialize(&r);
	if (error != 0) {
		if (r != NULL) delete r; //a part of tree was created
		return error;
	}
	tmpRes = r;
	free (buffer);
	return 0;
 }
  
int SimpleResultPackage::dataDeserialize(Result** r ) {
//	int error;
//	ResultBag *brs;
//	Result* result;	
	char* id;
	int error;
	unsigned long number;
//	unsigned long i;
	unsigned long* lptr;
	double db;
	char df;
		
	switch (*(bufferBegin++)) {
	
		case Result::BAG:
			cerr << "<Connection::deserialize> tworze obiekt BAG\n";
			*r = new ResultBag();
			return grabElements((ResultCollection*) *r);
			
		case Result::SEQUENCE:
			cerr << "<Connection::deserialize> tworze obiekt SEQUENCE\n";
			*r = new ResultSequence();
			return grabElements((ResultCollection*) *r);
		
		case Result::STRUCT:
			cerr << "<Connection::deserialize> tworze obiekt STRUCT\n";
			*r = new ResultStruct();
			return grabElements((ResultCollection*) *r);
			
		case Result::REFERENCE:
			cerr << "<Connection::deserialize> tworze obiekt REFERENCE\n";
			error = stringCopy(id); //by reference
			if (error != 0) return error;
			*r = new ResultReference(string (id));
			return 0;
		
		case Result::VOID:
		cerr << "<Connection::deserialize> tworze obiekt VOID\n";
			*r = new ResultVoid();
			return 0;
		
	        case Result::STRING:
		  cerr << "<Connection::deserialize> tworze obiekt STRING\n " ;
			error = stringCopy(id); // by reference
			if (error != 0) return error;
			*r = new ResultString(string (id));
			return 0;
		
		case Result::ERROR:
			cerr << "<Connection::deserialize> tworze obiekt ERROR\n";
			error = getULong(number); //by reference
			if (error != 0) return error;
			*r = new ResultError(number);
			return 0;
			
		case Result::INT:
			cerr << "<Connection::deserialize> tworze obiekt INT\n";
			error = getULong(number); //by reference
			if (error != 0) return error;
			*r = new ResultInt((int) number);
			return 0;
		
		case Result::BOOLTRUE:
			cerr << "<Connection::deserialize> tworze obiekt BOOL (true)\n";
			*r = new ResultBool(true);
			return 0;
		
		case Result::BOOLFALSE:
			cerr << "<Connection::deserialize> tworze obiekt BOOL (false)\n";
			*r = new ResultBool(false);
			return 0;
		
		case Result::DOUBLE:
			cerr << "<Connection::deserialize> tworze obiekt DOUBLE\n";
			lptr = (unsigned long*) &db;
			error = getULong(*lptr); //higher word
			if (error != 0) return error;
			error = getULong(*(lptr+1)); //lower word
			if (error != 0) return error;
			*r = new ResultDouble(db);
			return 0;
		
		case Result::BINDER:
			cerr << "<Connection::deserialize> tworze obiekt BINDER\n";
			error = stringCopy(id); // by reference
			if (error != 0) return error;
			Result* subRes;
			error = dataDeserialize(&subRes);
			if (error != 0) return error;
			*r = new ResultBinder(string(id), subRes);
			return 0;
			
		default:
			df = *(bufferBegin-1);
			cerr << "<Connection::deserialize> obiekt nieznany, nr: " << (int) df << endl;
			return -1; //TODO
	} // switch
} // deserialize

};

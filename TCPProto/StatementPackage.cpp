#include <netinet/in.h> 
#include "Package.h"

namespace TCPProto {

	int StatementPackage::serialize(char** buffer, int* size) {
		int length = sizeof(unsigned long) + 1; //including type
		
		*buffer = (char*) malloc(length); // including type
		if (*buffer == NULL) return -2; //TODO
		
		char type = (char) getType();
		memcpy(*buffer, &type, sizeof(char));
		
		bufferBegin = *buffer + 1;
		bufferEnd   = *buffer + length;
		
		int error  = setULong(stmtNr);
		if (error != 0) return -2;

		*size = length;
		return 0;
	}

	int StatementPackage::deserialize(char* buffer, int size) {

		char* tmpBuffer;
		if (buffer[0] != getType()) return -1; //TODO
		tmpBuffer = buffer + 1;
		
		bufferBegin = buffer + 1;
		bufferEnd   = buffer + size;

		int error  = getULong(&stmtNr);
		if (error != 0) return -2;

		free(buffer);
		return 0;
	}

	unsigned long StatementPackage::getSize() { //including NULL
		return sizeof(char) + sizeof(unsigned long);
	}

	unsigned long StatementPackage::getStmtNr() {
		return stmtNr;
	}

	void StatementPackage::setStmtNr(unsigned long stmtNr) {
		StatementPackage::stmtNr = stmtNr;
	}
	
	int StatementPackage::getULong(unsigned long *val, char** buffer) {
		char* tmpPtr = *buffer + sizeof(unsigned long);
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}

		*val     = ntohl(*((unsigned long*) *buffer));
		*buffer = tmpPtr; //skip the number of elements (long)
		return 0;
	}

	int StatementPackage::getULong(unsigned long *val) {
		
		char* tmpPtr = bufferBegin + sizeof(long);
		
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}
		
		// cerr << "getULong: " << *((unsigned long*) bufferBegin) << endl;
		*val = ntohl(*((unsigned long*) bufferBegin));
		bufferBegin = tmpPtr; //skip the number of elements (long)
		return 0;
	}

	int StatementPackage::setULong(unsigned long val) {
		char* tmpPtr = bufferBegin + sizeof(unsigned long);
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}
		
		// cerr << "setULong: " << val << endl;
		val = htonl(val);
		memcpy(bufferBegin, &val, sizeof(unsigned long));

		bufferBegin  = tmpPtr; //skip the number of elements (long)
		return 0;
	}

	int StatementPackage::setULong(unsigned long val, char** buffer) {
		char* tmpPtr = *buffer + sizeof(unsigned long);
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}

		val = htonl(val);
		memcpy(*buffer, &val, sizeof(unsigned long));
		//**buffer     = htonl(val);
		bufferBegin  = tmpPtr; //skip the number of elements (long)
		return 0;
	}
}

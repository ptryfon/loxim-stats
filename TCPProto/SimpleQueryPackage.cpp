#include "Package.h"

namespace TCPProto {

	int SimpleQueryPackage::serialize(char** buffer, int* size) {
		int length = strlen(tmpBuffer)+2; //including NULL and type
		*buffer = (char*) malloc(length); // including type
		if (*buffer == NULL) return -2; //TODO
		*buffer[0] = (char) getType();
		strcpy(*buffer+1, tmpBuffer);
		*size = length;
		return 0;
	}

	int SimpleQueryPackage::deserialize(char* buffer, int size) {
		if (buffer[0] != getType()) return -1; //TODO
		buf = (char*) malloc(size-1);
		if (buf == NULL) return -1; //TODO
		strcpy(buf, buffer+1);
		querySize = size - 1;
		free(buffer);
		return 0;
	}

	void SimpleQueryPackage::setQuery(const char* query) {
		tmpBuffer = query;
	}

	char* SimpleQueryPackage::getQuery() {
		return buf;
	}

	int SimpleQueryPackage::getQuerySize() { //including NULL
		return querySize;
	}
}

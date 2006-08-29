#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include "Package.h"
#include "../Driver/Result.h"
#include "../Errors/Errors.h"

#define ERR_SIZE 4
#define ERR_PACK_SIZE ERR_SIZE+1

namespace TCPProto {

	void ErrorPackage::init() {
	    bufferB = (char *)malloc(ERR_PACK_SIZE);
	    //TODO -> nomem
	    memset(bufferB, '\0', ERR_PACK_SIZE);
	    serialBuf=bufferB;
	    serialBuf[0] = (char)Package::ERRORRESULT;
	    serialBuf++;
	    errorNo = 0;	    
	}
	
	void ErrorPackage::deinit() {
	    free(bufferB);
	}

	int ErrorPackage::serialize(char** buffer, int* size) {
		ec = new ErrorConsole("ErrorPackage");
		ec->printf("ErrorPackage - serialize: Sending error number %d ...\n", errorNo);
		errorNo = htonl(errorNo);
		memcpy((void *)serialBuf, (const void *)&errorNo, sizeof(errorNo)); 
		*buffer = &*bufferB;
		*size = ERR_PACK_SIZE;
		delete ec;
		return 0;
	}

	int ErrorPackage::deserialize(char* buffer, int size) {
		if (buffer[0] != getType()) return -1;
		memcpy((void *)&errorNo, (const void *)(buffer+1), ERR_SIZE);
		errorNo = ntohl(errorNo); 
		//ec->printf("ErrorPackage - deserialize: Received error number %d ...\n", errorNo);
		tmpRes = new ResultError(errorNo);
		//free(buffer);
		return 0;
	}

	void ErrorPackage::setError(int errNo) {
		errorNo = errNo;
	}

	int ErrorPackage::getError() {
		return errorNo;
	}
	
	Result *ErrorPackage::getResult() {
		return tmpRes;
	}
}

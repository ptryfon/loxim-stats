#include <netinet/in.h> 
#include "Package.h"

namespace TCPProto {
	
	int RemoteQueryPackage::stringCopy(char* &newBuffer) { // od bufferBegin
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
	
	int RemoteQueryPackage::serialize(char** buffer, int* size) {
		int s = 2;
		int len;
		if (lid != NULL) {
			cerr << "nie null\n";
			len = lid->toString().length()+1;
			s += len;
		}
		*size = s;
		*buffer = (char*) malloc(*size);
		cerr << "malloc ok\n";
		char* buf = *buffer;
		buf[0] = (char) getType();
		buf[1] = deref ? 1 : 0;
		cerr << "typ ok\n";
		if (lid == NULL) {
			cerr << "null, koncze ok\n";
			return 0;
		}
		const char* remoteLid = lid -> getRemoteID()->toString().c_str();
		cerr << "TCPProto - serializacja RemoteQueryPackage logical ID: " << remoteLid << "\n";
		(buf) += 2;
		strcpy(buf, remoteLid);
		(buf)+= len;
		return 0;
	}

	int RemoteQueryPackage::deserialize(char* buffer, int size) {
		if (buffer[0] != getType()) return -1; //TODO
		deref = (buffer[1] == 1);
		bufferEnd = buffer+size;
		bufferBegin = buffer;
		bufferBegin+=2;
		if (size == 2) {
			lid = NULL;
			deref = false;
		} else {
			char* id;
			stringCopy(id);
			int nr = atoi(id);
			cerr << "TCPProto deserializacja remoteQueryPackage. zawiera logical ID: " << nr << endl;
			lid = new DBLogicalID(nr);
		}
		free(buffer);
		return 0;
	}

}

#include "Package.h"

namespace TCPProto {

	int RemoteQueryPackage::serialize(char** buffer, int* size) {
		*size = 1;
		*buffer = (char*) malloc(*size);
		*buffer[0] = (char) getType();
		return 0;
	}

	int RemoteQueryPackage::deserialize(char* buffer, int size) {
		free(buffer);
		return 0;
	}

}

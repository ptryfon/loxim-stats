#include <netinet/in.h> 
#include "Package.h"

namespace TCPProto {

int ParamQueryPackage::serialize(char** buffer, int* size) {
	return SimpleQueryPackage::serialize(buffer, size);
}

int ParamQueryPackage::deserialize(char* buffer, int size) {
	return SimpleQueryPackage::deserialize(buffer, size);
}

}// namespace TCPProto

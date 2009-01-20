#ifndef PROTOCOL_EXCEPTIONS_H
#define PROTOCOL_EXCEPTIONS_H

#include <Errors/Exceptions.h>

namespace Protocol{
	class OperationCancelled : public Errors::LoximException {
		public:
			OperationCancelled();

	};

	class ReadError : public Errors::LoximException {
		public:
			ReadError(int error);

	};
	
	class WriteError : public Errors::LoximException {
		public:
			WriteError(int error);

	};
	
	class ConnectionError : public Errors::LoximException {
		public:
			ConnectionError(int error);

	};
	
	class ProtocolLogic : public Errors::LoximException {
		public:
			ProtocolLogic();

	};
	
	class PackageTooBig : public Errors::LoximException {
		public:
			PackageTooBig();

	};
}

#endif /* PROTOCOL_EXCEPTIONS_H */ 

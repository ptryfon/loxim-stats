#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "Result.h"

namespace Driver {

using namespace std;


class ConnectionException //Czarek
{
protected:
	string msg;
	int error;
public:
	ConnectionException(int err, string msg) : msg(msg), error(err) {};
	ConnectionException() : error(-1) { msg = " Undefined exception "; }
	ConnectionException(string msg) : msg(msg), error(-1) {}
	ConnectionException(int err) : msg(""), error(err) {}
	virtual void toStream(ostream& os) const { os << msg << " error code: " << error;  }
	friend ostream& operator<<(ostream&, ConnectionException&);
	virtual int getError() { return error; }
	virtual ~ConnectionException() {}
};

class ClosedConnectionException : public ConnectionException
{
public:
	ClosedConnectionException() : ConnectionException("server was closed") {};
};

class ProtocolCorruptException : public ConnectionException
{
public:
	ProtocolCorruptException() : ConnectionException("protocol corrupt") {};
};

class TransmissionException : public ConnectionException
{
public:
	TransmissionException(int err) : ConnectionException(err, "transmission exception") {};
};

class ServerException : public ConnectionException
{
public:
	ServerException(int err) : ConnectionException(err, "server error") {};
};

class MemoryException : public ConnectionException
{
public:
	MemoryException(int err) : ConnectionException(err, "memory exception") {};
};

class Connection
{
public:
	Connection(int socket);
	int disconnect();
	virtual ~Connection();
	Result* execute(const char* query) throw (ConnectionException);
private:
	int sock;
	char* bufferBegin;
	char* bufferEnd;
	
	unsigned long getULong(unsigned long &val);
	int stringCopy(char* &newBuffer);
	Result* grabElements(ResultCollection* col);
	Result* deserialize();
};

class BufferHandler
{
	public:
		char* buf;
		BufferHandler(char* buffer): buf(buffer) {};
		~BufferHandler() {free (buf);};
};

} // namespace

#endif //_CONNECTION_H_

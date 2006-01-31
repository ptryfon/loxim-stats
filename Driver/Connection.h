#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "Result.h"
#include "../Errors/Errors.h"

namespace Driver {

using namespace std;

	
class ConnectionException {
 protected:
	string msg;
 public: 
	ConnectionException() {}
	ConnectionException(string msg) : msg(msg) {}
	virtual void toStream(ostream& os) const { os << msg; }
	friend ostream& operator<<(ostream&, ConnectionException&);
	virtual ~ConnectionException() {}
};


class ConnectionErrnoException : public ConnectionException {
 protected:
	int error;
 public:
	ConnectionErrnoException(int err): ConnectionException(), error(err) 
	  { 
	    char cherr[256];
	    sprintf(cherr, "Error nr: 0x%x , ", err);
	    msg = string(cherr);
	    msg += string(strerror (err & 0xFFFF)); 
	  }
	virtual int getError() { return error; }
	virtual ~ConnectionErrnoException() {}
};

class ConnectionIOException : public ConnectionErrnoException {
 public:
	ConnectionIOException(int err): ConnectionErrnoException(err){}
};

class ConnectionMemoryException : public ConnectionErrnoException {
 public:
	ConnectionMemoryException(int err): ConnectionErrnoException(err){}

};

class ConnectionServerException : public ConnectionErrnoException {
 public:
	ConnectionServerException(int err): ConnectionErrnoException(err)
	  { 
	    char cherr[256];
	    sprintf(cherr, "Error nr: 0x%x , ", err);
	    msg = string(cherr);
	    
	    int module = err & 0xFF00000;
	    switch (module) {
	    case ErrBackup:
	      msg += string ("Parser Error");
	      break; 
	    case ErrConfig:
	      msg += string ("Config Error");
	      break; 
	    case ErrDriver:
	      msg += string ("Driver Error");
	      break; 
	    case ErrErrors:
	      msg += string ("Errors Error");
	      break; 
	    case ErrLockMgr:
	      msg += string ("Lock Manager Error");
	      break; 
	    case ErrLogs:
	      msg += string ("Logs Error");
	      break; 
	    case ErrQExecutor:
	      msg += string ("Query Executor Error");
	      break; 
	    case ErrQParser:
	      msg += string ("Query Parser Error");
	      break; 
	    case ErrSBQLCli:
	      msg += string ("SBQL Client Error");
	      break; 
	    case ErrServer:
	      msg += string ("Server Error");
	      break; 
	    case ErrStore:
	      msg += string ("Store Error");
	      break; 
	    case ErrTManager:
	      msg += string ("Transaction Manager Error");
	      break; 
	    case ErrTCPProto:
	      msg += string ("Protocol Error");
	    }
	  }
	
};


class ConnectionClosedException : public ConnectionException {
 public:
	virtual void toStream(ostream& os) const 
		{ os << "Closed connection Exception "; }
};


class ConnectionProtocolException : public ConnectionException {
 public:
	virtual void toStream(ostream& os) const 
		{ os << "Protocol Corrupt  Exception "; }
};


class ConnectionDriverException : public ConnectionException {
 public:
	ConnectionDriverException() {}
	ConnectionDriverException(string msg) : 
		ConnectionException(msg) {}
};



#if 0
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

//TODO transmission i to co jest w drivermanager jako ConnectionIOException

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


#endif

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

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "Result.h"

namespace Driver {

using namespace std;


class ConnectionException //Czarek
{
private:
	string msg;
public:
	ConnectionException() { msg = " Undefined exception "; }
	ConnectionException(string msg) : msg(msg) {}
	void   toStream(ostream& os) const { os << msg;  }
	friend ostream& operator<<(ostream&, ConnectionException&);
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

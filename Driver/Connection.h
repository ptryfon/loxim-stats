#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "Result.h"

using namespace std;

class Connection
{
public:
	Connection(int socket);
	int disconnect();
	virtual ~Connection();
	Result* execute(char* query);
private:
	int sock;
	char* bufferBegin;
	char* bufferEnd;
	
	int getULong(unsigned long &val);
	int stringCopy(char* &newBuffer);
	int deserialize(Result** rs);
};


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

#endif //_CONNECTION_H_

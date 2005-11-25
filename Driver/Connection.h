#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "Result.h"

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

#endif //_CONNECTION_H_

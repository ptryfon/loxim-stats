#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "ResultSet.h"

class Connection
{
public:
	Connection(int socket);
	int disconnect();
	virtual ~Connection();
	ResultSet* execute(char* query);
private:
	int sock; 
};

#endif //_CONNECTION_H_

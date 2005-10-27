#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "ResultSet.h"
#include <winsock2.h>

class Connection {
public:
	Connection(SOCKET sock);
	virtual ~Connection();
	ResultSet* execute(char* query);
	void disconnect();
};

#endif /*CONNECTION_H_*/

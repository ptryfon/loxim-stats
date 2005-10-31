#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "ResultSet.h"
#include <winsock2.h>
#include <string>

class Connection {
public:
	Connection(SOCKET sock);
	virtual ~Connection();
	ResultSet* execute(string query);
	void disconnect();
};

#endif /*CONNECTION_H_*/

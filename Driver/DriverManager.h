#ifndef DRIVERMANAGER_H_
#define DRIVERMANAGER_H_

#include "Connection.h"

class DriverManager
{
public:
	DriverManager();
	virtual ~DriverManager();
	static Connection*	getConnection(char* url, int port);
};

#endif /*DRIVERMANAGER_H_*/

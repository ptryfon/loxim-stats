#ifndef _DRIVERMANAGER_H_
#define _DRIVERMANAGER_H_

#include "Connection.h"

class DriverManager
{
public:
	DriverManager();
	static Connection* getConnection(char* url, int port) 
	  throw (ConnectionException);
	virtual ~DriverManager();
};

#endif //_DRIVERMANAGER_H_

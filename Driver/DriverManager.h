#ifndef _DRIVERMANAGER_H_
#define _DRIVERMANAGER_H_

#include "Connection.h"

class DriverManager
{
public:
	DriverManager();
	Connection* getConnection(char* url, int port);
	virtual ~DriverManager();
};

#endif //_DRIVERMANAGER_H_

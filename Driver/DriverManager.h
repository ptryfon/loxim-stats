#ifndef DRIVERMANAGER_H_
#define DRIVERMANAGER_H_

#include "Connection.h"
#include <string>

class DriverManager
{
public:
	DriverManager();
	virtual ~DriverManager();
	static Connection*	getConnection(string url, int port);
};

#endif /*DRIVERMANAGER_H_*/

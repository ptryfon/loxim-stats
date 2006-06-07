#ifndef _DRIVERMANAGER_H_
#define _DRIVERMANAGER_H_

#include "Connection.h"

namespace Driver {

class DriverManager
{
public:
	DriverManager();
	 static Connection* getConnection(char* url, int port)
	  throw (ConnectionDriverException);
	virtual ~DriverManager();
};

} // namespace

#endif //_DRIVERMANAGER_H_

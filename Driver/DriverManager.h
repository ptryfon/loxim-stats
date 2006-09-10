#ifndef _DRIVERMANAGER_H_
#define _DRIVERMANAGER_H_

#include "Connection.h"

namespace Driver {

class DriverManager
{
public:
	DriverManager();
	//ver --> czy weryfikowac? Jesli wolisz moze byc oddzielna funkcja.. (Adam)
	 static Connection* getConnection(char* url, int port, const char* login, const char* passwd, int ver)
	  throw (ConnectionDriverException);
	virtual ~DriverManager();
};

} // namespace

#endif //_DRIVERMANAGER_H_

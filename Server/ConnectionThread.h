#ifndef CONNECTIONTHREAD_H_
#define CONNECTIONTHREAD_H_

#include <winsock2.h>

class ConnectionThread
{
public:
	ConnectionThread(SOCKET nowy);
	virtual ~ConnectionThread();
	int run();
private:
	SOCKET nowy;	
};

#endif /*CONNECTIONTHREAD_H_*/

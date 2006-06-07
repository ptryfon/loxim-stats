#include <stdio.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include "DriverManager.h"
#include "Connection.h"
#include "../TCPProto/Tcp.h"

namespace Driver {

using namespace std;

DriverManager::DriverManager() {}

DriverManager::~DriverManager() {}

Connection* DriverManager::getConnection(char* url, int port)
  throw (ConnectionDriverException)
{
	int newSock, error;
	error = TCPProto::getConnection(url, port, &newSock);
	if (error != 0)
	{
		throw ConnectionDriverException(" can't establish connection ");
	}
	return new Connection(newSock);
}
/*
  int main() {
  Connection* c = (new DriverManager())->getConnection("green",6543);
  c->disconnect();
  return 0;
  }
*/
} // namespace

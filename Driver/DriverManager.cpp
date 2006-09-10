#include <stdio.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>

#include "DriverManager.h"
#include "Connection.h"
#include "../TCPProto/Tcp.h"

using namespace std;

namespace Driver {

using namespace std;

DriverManager::DriverManager() {}

DriverManager::~DriverManager() {}

Connection* DriverManager::getConnection(char* url, int port, const char* login, const char* passwd, int ver)
  throw (ConnectionDriverException)
{
	int newSock, error;
	error = TCPProto::getConnection(url, port, &newSock);
	if (error != 0)
	{
		throw ConnectionDriverException(" can't establish connection ");
	}
	Connection *con = new Connection(newSock);
	/* weryfikacja loginu i hasla */
	if (ver==1) {
	//cerr << "Weyfikuje login i haslo!\n";	
	try {
	    con->set_print_err(false);
	    con->execute("begin");
	    string query = string("validate ") + string(login) + string(" ") + string(passwd) + ";";
	    Result *result = con->execute(query.c_str());	
	    con->execute("end");
	    int i_res = result->getType();
	    bool b_res = false;
	    if (i_res == Result::BOOL) {
		ResultBool *boolResult = (ResultBool *) result;
		b_res = boolResult->getValue();
	    }	    
	    if (b_res == false)
		throw ConnectionDriverException("user validation failed(check login or password) ");

	    con->set_print_err(true);	
	} 
	catch (ConnectionServerException e)
	{		
	    con->execute("end");
	    cerr << e << endl;
	    throw ConnectionDriverException("user validation failed(check login or password) ");
	}
	}
	return con;
}
/*
  int main() {
  Connection* c = (new DriverManager())->getConnection("green",6543);
  c->disconnect();
  return 0;
  }
*/
} // namespace

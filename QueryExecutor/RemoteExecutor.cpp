#include "RemoteExecutor.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "QueryResult.h"
#include "Errors/Errors.h"
#include "../TCPProto/Tcp.h"

using namespace TCPProto;
using namespace std;

namespace QExecutor {

	RemoteExecutor::RemoteExecutor(string ip, int port) {
		this->port = port;
		this->ip = ip;
		ec = new ErrorConsole("RemoteExecutor");
	}
	
	int RemoteExecutor::connect(const char* ip, int port) {
		int newSock, err;
		err = TCPProto::getConnection(ip, port, &newSock);
		if (err != 0) {
			return err;
		}
		//Connection *con = new Connection(newSock);
		//delete con;
		return 0;
	}
	
	
	RemoteExecutor::~RemoteExecutor() {}
	int RemoteExecutor::execute(QueryResult **qr) {
		
		*qr = new QueryIntResult(5);
		*ec << "zaczynam remote execute, ip: ";
		*ec << ip;
		*ec << "port: "; *ec << port;
		
		return connect(ip.c_str(), port);
	
	}
	
}

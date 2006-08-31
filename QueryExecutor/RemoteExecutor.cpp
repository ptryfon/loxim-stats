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
#include "SessionData.h"
#include "QueryExecutor.h"

using namespace TCPProto;
using namespace std;

namespace QExecutor {

	RemoteExecutor::RemoteExecutor(string ip, int port, QueryExecutor* qe) {
		this->port = port;
		this->ip = ip;
		this->qe = qe;
		ec = new ErrorConsole("RemoteExecutor");
	}
	
	int RemoteExecutor::connect(const char* ip, int port, int *sock) {
	*ec << "zaczynam connect, ip";
	*ec << ip;
	ec->printf("port: %d", port);
		int newSock, err;
		string q;
		err = TCPProto::getConnection(ip, port, &newSock);
		if (err != 0) {
			return err;
		}
		
		*ec << "mam connection";
		SimpleQueryPackage spackage;
		Package *package;
		//cerr << "zaczynam execute\n";
		spackage.setQuery("begin");
		err = packageSend(&spackage, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "wyslalem 1";
		err = packageReceive(&package, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "odebralem 1";
		string login= qe->session_data->get_user_data()->get_login();
		string passwd = qe->session_data->get_user_data()->get_passwd();
		
		q = string("validate ") + string(login) + string(" ") + string(passwd) + ";";
		*ec << "wysylam 2, login";
		*ec << login;
		*ec << passwd;
		spackage.setQuery(q.c_str());
		err = packageSend(&spackage, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "wyslalem 2";
		err = packageReceive(&package, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "odebralem 2";
		spackage.setQuery("end");
		err = packageSend(&spackage, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "wyslalem 3";
		err = packageReceive(&package, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "odebralem 3";
		*sock = newSock;
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
		
		int sock;
		
		return connect(ip.c_str(), port, &sock);
	
	}
	
}

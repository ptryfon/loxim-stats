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
//#include "../TCPProto/TCPParam.h"
//#include "../TCPProto/Package.h"
#include "QueryExecutor.h"
#include "LoximServer/LoximServer.h"
#define SLEEP			10000
#define CONNECT_SLEEP	10000

using namespace TCPProto;
using namespace std;

namespace QExecutor {

	RemoteExecutor::RemoteExecutor(string ip, int port, QueryExecutor* qe) {
		this->port = port;
		this->ip = ip;
		this->qe = qe;
		ec = new ErrorConsole("RemoteExecutor");
		lid = NULL;
		deref = false;
	}
	
	void RemoteExecutor::setDeref() {
		deref = true;
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
		
		usleep(CONNECT_SLEEP);
		
		*ec << "mam connection";
		SimpleQueryPackage spackage;
		TCPProto::Package *package;
		//cerr << "zaczynam execute\n";
		spackage.setQuery("begin");
		err = packageSend(&spackage, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "wyslalem 1";
		usleep(SLEEP);
		err = packageReceive(&package, newSock);
		if (err != 0) {
			return err;
		}
		*ec << "odebralem 1";
		
		usleep(SLEEP);
		
		string login= qe->session->get_user_data()->get_login();
		string passwd = qe->session->get_user_data()->get_passwd();
		
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
		
		usleep(SLEEP);
		
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
		
		usleep(SLEEP);
		
		*ec << "odebralem 3";
		//autentykacja zakonczona, zaczynam transakcje operacji
		spackage.setQuery("begin");
		err = packageSend(&spackage, newSock);
		if (err != 0) {
			return err;
		}
		
		err = packageReceive(&package, newSock);
		if (err != 0) {
			return err;
		}
		
		usleep(SLEEP);
		
		*sock = newSock;
		//Connection *con = new Connection(newSock);
		//delete con;
		return 0;
	}
	/*
	void RemoteExecutor::setLogicalID(LogicalID* lid) {
		this.lid = lid;
	}
	*/
	RemoteExecutor::~RemoteExecutor() {}
	int RemoteExecutor::execute(QueryResult **qr) {
		
		//*qr = new QueryIntResult(5);
		*ec << "zaczynam remote execute, ip: ";
		*ec << ip;
		*ec << "port: "; *ec << port;
		
		int sock;
		int err;
			
		usleep(SLEEP);
		
		err = connect(ip.c_str(), port, &sock);
		if (err != 0) {
			return err;
		}
		*ec << "wysylam paczke remote";
		RemoteQueryPackage *rqp = new RemoteQueryPackage();
		rqp->setLogicalID(lid);
		rqp->setDeref(deref);
		
		usleep(SLEEP);
		
		err = packageSend(rqp, sock);
		if (err != 0) {
			return err;
		}
		
		usleep(SLEEP);
		
		TCPProto::Package* package;
		TCPParam* param = new TCPParam();
		param->setServer(ip);
		param->setPort(port);
		err = packageReceive(&package, sock, (void*)param, TCPProto::Package::REMOTERESULT);
		if (err != 0) {
			return err;
		}
		delete param;
		*ec << "odebralem sekcje stosu";
		RemoteResultPackage *rrp;
		rrp = (RemoteResultPackage*) (package);
		*qr = rrp->getQueryResult();
		
		usleep(SLEEP);
		
		SimpleQueryPackage spackage;
		spackage.setQuery("end");
		err = packageSend(&spackage, sock);
		if (err != 0) {
			return err;
		}
		*ec << "koncze sesje";
		err = packageReceive(&package, sock);
		if (err != 0) {
			return err;
		}
		
		*ec << "sesja zakonczona";
		
		err = close(sock);
		if (err != 0) {
			*ec << "blad zamykania gniazda";
		} else {
			*ec << "poprawnie zakonczylem remote executor";
		}
		
		return 0;
	
	}
	
}

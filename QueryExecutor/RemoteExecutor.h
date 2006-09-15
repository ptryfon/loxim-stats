#ifndef _REMOTEEXECUTOR_H
#define _REMOTEEXECUTOR_H

#include "QueryResult.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"
#include "QueryExecutor.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

namespace QExecutor
{
	class RemoteExecutor
	{
	protected:
		int port;
		string ip;
		QueryExecutor *qe;
		LogicalID *lid;
		bool deref;
		ErrorConsole *ec;
		
		int connect(const char* ip, int port, int*sock);

	public:
		RemoteExecutor(string ip, int port, QueryExecutor* qe);
		~RemoteExecutor();
		int execute(QueryResult** qr);
		void setLogicalID(LogicalID* lid) {this->lid = lid;}
		void setDeref();
	};
	
}

#endif

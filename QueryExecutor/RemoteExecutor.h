#ifndef _REMOTEEXECUTOR_H
#define _REMOTEEXECUTOR_H

#include "QueryResult.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"
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
		
		ErrorConsole *ec;
		
		int connect(const char* ip, int port);

	public:
		RemoteExecutor(string ip, int port);
		~RemoteExecutor();
		int execute(QueryResult** qr);
	};
	
}

#endif

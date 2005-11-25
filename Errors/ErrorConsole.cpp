#include <fstream>
#include <iostream>
#include "ErrorConsole.h"
#include "Errors.h"

using namespace std;

namespace Errors {
	ofstream* ErrorConsole::consoleFile = NULL;
	int ErrorConsole::nObjects = 0;

	ErrorConsole::ErrorConsole()
       	{
		if (consoleFile == NULL)
			consoleFile = new ofstream("sbqlerror.log", ios::app);
//		if (!consoleFile->is_open())
//			return -EOPEN;
		nObjects++;
	};

	ErrorConsole::ErrorConsole(string module)
	{
		if (consoleFile == NULL)
			consoleFile = new ofstream("sbqlerror.log", ios::app);
		nObjects++;
	};

	void ErrorConsole::operator<<(int error)
       	{
		switch(error & ErrAllModules) {
			case ErrBackup:
				*consoleFile << "Backup: ";
				break;
			case ErrConfig:
				*consoleFile << "Config: ";
				break;
			case ErrDriver:
				*consoleFile << "Driver: ";
				break;
			case ErrErrors:
				*consoleFile << "Errors: ";
				break;
			case ErrLockMgr:
				*consoleFile << "LockMgr: ";
				break;
			case ErrLogs:
				*consoleFile << "Logs: ";
				break;
			case ErrQExecutor:
				*consoleFile << "QExecutor: ";
				break;
			case ErrQParser:
				*consoleFile << "QParser: ";
				break;
			case ErrSBQLCli:
				*consoleFile << "SBQLCli: ";
				break;
			case ErrServer:
				*consoleFile << "Server: ";
				break;
			case ErrStore:
				*consoleFile << "Store: ";
				break;
			case ErrTManager:
				*consoleFile << "TManager: ";
				break;
			default:
				*consoleFile << "Aieeeee! \"Unknown\" returned: ";
				break;
		}
		*consoleFile << "errno: ";
		*consoleFile << (error & ~ErrAllModules);
		*consoleFile << endl;
	};

	void ErrorConsole::operator<<(string errorMsg)
       	{
		*consoleFile << errorMsg;
	};

	ErrorConsole::~ErrorConsole()
       	{
		nObjects--;
		if (nObjects == 0) {
			delete consoleFile;
			consoleFile = NULL;
		}
	};
}

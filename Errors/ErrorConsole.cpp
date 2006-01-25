#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <ios>
#include "ErrorConsole.h"
#include "Errors.h"

using namespace std;

namespace Errors {
	ofstream* ErrorConsole::consoleFile = NULL;
	int ErrorConsole::nObjects = 0;
	int ErrorConsole::serr = 0;

	ErrorConsole::ErrorConsole()
       	{
//		owner();
		nObjects++;
	};

	ErrorConsole::ErrorConsole(string module)
	{
		owner = module;
		nObjects++;
	};

//	int ErrorConsole::init(void)
	int ErrorConsole::init(int tostderr)
	{
		serr = tostderr;
		if (consoleFile == NULL)
			consoleFile = new ofstream("sbqlerror.log", ios::app);
//		if (!consoleFile->is_open())
//			return EOPEN;
		return 0;
	};

	ErrorConsole& ErrorConsole::operator<<(int error)
       	{
		string modname;

		switch(error & ErrAllModules) {
			case ErrBackup:
				modname = "Backup: ";
				break;
			case ErrConfig:
				modname = "Config: ";
				break;
			case ErrDriver:
				modname = "Driver: ";
				break;
			case ErrErrors:
				modname = "Errors: ";
				break;
			case ErrLockMgr:
				modname = "LockMgr: ";
				break;
			case ErrLogs:
				modname = "Logs: ";
				break;
			case ErrQExecutor:
				modname = "QExecutor: ";
				break;
			case ErrQParser:
				modname = "QParser: ";
				break;
			case ErrSBQLCli:
				modname = "SBQLCli: ";
				break;
			case ErrServer:
				modname = "Server: ";
				break;
			case ErrStore:
				modname = "Store: ";
				break;
			case ErrTManager:
				modname = "TManager: ";
				break;
			default:
				if (owner.length() == 0)
					modname = "Aieeeee! \"Unknown\" returned: ";
				else
					modname = owner + ": ";
				break;
		}
		*consoleFile << modname << "errno: " << (error & ~ErrAllModules) << "\n";
		consoleFile->flush();
		if (serr) {
			cerr << modname << "errno: " << (error & ~ErrAllModules) << "\n";
			cerr.flush();
		}
		return *this;
	};

	ErrorConsole& ErrorConsole::operator<<(string errorMsg)
       	{
		*consoleFile << errorMsg;
		consoleFile->flush();
		if (serr) {
			cerr << errorMsg;
			cerr.flush();
		}
		return *this;
	};

	ErrorConsole& ErrorConsole::operator<<(ErrorConsole &cons)
       	{
		*consoleFile << cons.consoleFile;
		consoleFile->flush();
		if (serr) {
			cerr << cons.consoleFile;
			cerr.flush();
		}
		return *this;
	};

	ErrorConsole& ErrorConsole::printf(const char *format, ...)
	{
		char str[1024];
		va_list ap;

		va_start(ap, format);
		vsnprintf(str, 1024, format, ap);
		va_end(ap);

		*consoleFile << str;
		consoleFile->flush();
		if (serr) {
			cerr << str;
			cerr.flush();
		}
		return *this;
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

#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <ios>
#include "ErrorConsole.h"
#include "Errors.h"
#include "Config/SBQLConfig.h"

using namespace std;
using namespace Config;

namespace Errors {
	extern char *ErrorMsg[];

	ofstream* ErrorConsole::consoleFile = NULL;
	int ErrorConsole::serr = 0;

	ErrorConsole::ErrorConsole()
       	{
	};

	ErrorConsole::ErrorConsole(string module)
	{
		owner = module;
	};

//	int ErrorConsole::init(void)
	int ErrorConsole::init(int tostderr)
	{
		SBQLConfig c("Errors");
		string f;

		c.getInt("stderr", serr);
		if (c.getString("logfile", f) != 0)
			f = "sbqlerror.log";
		serr = tostderr;
		if (consoleFile == NULL)
			consoleFile = new ofstream(f.c_str(), ios::app);
		if (!consoleFile->is_open())
			return ENoFile | ErrErrors;
		return 0;
	};

	ErrorConsole& ErrorConsole::operator<<(int error)
       	{
		string src_mod, dest_mod, str;

		switch(error & ErrAllModules) {
			case ErrBackup:
				src_mod = "Backup";
				break;
			case ErrConfig:
				src_mod = "Config";
				break;
			case ErrDriver:
				src_mod = "Driver";
				break;
			case ErrErrors:
				src_mod = "Errors";
				break;
			case ErrLockMgr:
				src_mod = "LockMgr";
				break;
			case ErrLogs:
				src_mod = "Logs";
				break;
			case ErrQExecutor:
				src_mod = "QExecutor";
				break;
			case ErrQParser:
				src_mod = "QParser";
				break;
			case ErrSBQLCli:
				src_mod = "SBQLCli";
				break;
			case ErrServer:
				src_mod = "Server";
				break;
			case ErrStore:
				src_mod = "Store";
				break;
			case ErrTManager:
				src_mod = "TManager";
				break;
			default:
				src_mod = "Unknown";
				break;
		}
		if ((error & ~ErrAllModules) < 0x100) {
			str = strerror(error & ~ErrAllModules);
		} else {
			int erridx = (error & ~ErrAllModules);
			erridx = erridx < EUnknown ? erridx / 0x100 : EUnknown / 0x100;
			str = ErrorMsg[erridx];
		}
		if (owner.length() == 0)
			dest_mod = "Unknown: ";
		else
			dest_mod = owner + ": ";

		*consoleFile << dest_mod << src_mod << " said: " << str << " (errno: "<< (error & ~ErrAllModules) << ")\n";
		consoleFile->flush();
		if (serr) {
			cerr << dest_mod << src_mod << " said: " << str << " (errno: "<< (error & ~ErrAllModules) << ")\n";
			cerr.flush();
		}
		return *this;
	};

	ErrorConsole& ErrorConsole::operator<<(string errorMsg)
       	{
		string dest_mod;

		if (owner.length() == 0)
			dest_mod = "Unknown: ";
		else
			dest_mod = owner + ": ";

		*consoleFile << dest_mod << errorMsg << "\n";
		consoleFile->flush();
		if (serr) {
			cerr << dest_mod << errorMsg << "\n";
			cerr.flush();
		}
		return *this;
	};

	ErrorConsole& ErrorConsole::printf(const char *format, ...)
	{
		char str[1024];
		va_list ap;
		string dest_mod;

		va_start(ap, format);
		vsnprintf(str, 1024, format, ap);
		va_end(ap);

		if (owner.length() == 0)
			dest_mod = "Unknown: ";
		else
			dest_mod = owner + ": ";

		*consoleFile << dest_mod << str;
		consoleFile->flush();
		if (serr) {
			cerr << dest_mod << str;
			cerr.flush();
		}
		return *this;
	};

	void ErrorConsole::free(void)
       	{
		if (consoleFile != NULL) {
			delete consoleFile;
			consoleFile = NULL;
		}
	};

	ErrorConsole::~ErrorConsole()
       	{
	};
}

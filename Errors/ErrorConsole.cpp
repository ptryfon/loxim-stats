#include <fstream>
#include <iostream>
#include "ErrorConsole.h"

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
		*consoleFile << error;
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

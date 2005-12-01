#ifndef _ERRORCONSOLE_H
#define _ERRORCONSOLE_H

#include <fstream>
#include <string>

using namespace std;

namespace Errors {
	class ErrorConsole {
		private:
			static ofstream* consoleFile;
			static int nObjects;
			string owner;
			int serr;
		public:
			ErrorConsole();
			ErrorConsole(string module);

			int init(int tostderr);
			virtual int operator<<(int error);
			virtual string operator<<(string errorMsg);
			virtual ~ErrorConsole();
	};
}
#endif

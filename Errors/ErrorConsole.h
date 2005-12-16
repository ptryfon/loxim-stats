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
			static int serr;
			string owner;
		public:
			ErrorConsole();
			ErrorConsole(string module);

			int init(int tostderr);
			virtual ErrorConsole& operator<<(int error);
			virtual ErrorConsole& operator<<(string errorMsg);
			virtual ErrorConsole& operator<<(ErrorConsole &cons);
			virtual ~ErrorConsole();
	};

	inline ErrorConsole &endl(ErrorConsole &s) { s << "\n"; return s; }
}
#endif

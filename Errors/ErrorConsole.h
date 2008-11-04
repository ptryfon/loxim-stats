#ifndef _ERRORCONSOLE_H
#define _ERRORCONSOLE_H

#include <fstream>
#include <string>

using namespace std;

namespace Errors {
	class ErrorConsole {
		private:
			static ofstream* consoleFile;
			static int serr;
			string owner;
                        static bool useLogFile;
		public:
			ErrorConsole();
			ErrorConsole(string module);

			//int init(int tostderr);
                        int init();
			void free(void);
			virtual ErrorConsole& operator<<(int error);
			virtual ErrorConsole& operator<<(const string &errorMsg);
			virtual ErrorConsole& printf(const char *format, ...);
			virtual ~ErrorConsole();
	};
}
#endif

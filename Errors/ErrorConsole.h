#include <fstream>
#include <string>

using namespace std;

namespace Errors {
	class ErrorConsole {
		private:
			static ofstream* consoleFile;
			static int nObjects;
		public:
			ErrorConsole();
			ErrorConsole(char *module);

			virtual void operator<<(int error);
			virtual void operator<<(string errorMsg);
			virtual ~ErrorConsole();
	};
}

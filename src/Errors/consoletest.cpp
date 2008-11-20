#include <iostream>
#include "ErrorConsole.h"
#include "Errors.h"
#include "Config/SBQLConfig.h"

using namespace std;
using namespace Errors;
using namespace Config;

int main(int argc, char *argv[])
{
	ErrorConsole ec("Config");
	SBQLConfig c("Errors");

	c.init("../Server/Server.conf");
	ec.init(1);
// przyklady uzycia
	ec << "aaaa";
	ec << "dupa1 " << " xxx";
	ec << "dupa2 " << ENoFile;
	ec << "dupa3 " << ENoFile << " xxx";
// to dzialac nie bedzie (i tak jest zle bo endl robi flush)
//	ec << endl;
	ec << (EBadFile | ErrStore);
	ec << (ENoFile);
	ec << 0xFF00;
	ec << (ENOMEM | ErrLogs);
	ec.printf("1: %d, ptr: %p\n", 1, NULL);
	cerr << SBQLstrerror(EBadFile | ErrStore)->c_str() << "\n";
	cerr << SBQLstrerror(0xFF00)->c_str() << "\n";
}

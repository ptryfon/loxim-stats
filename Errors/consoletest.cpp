#include <iostream>
#include "ErrorConsole.h"
#include "Errors.h"

using namespace std;
using namespace Errors;

int main(int argc, char *argv[])
{
	ErrorConsole ec("Config");

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
	ec << (ENOMEM | ErrLogs);
	ec.printf("1: %d, ptr: %p\n", 1, NULL);
}

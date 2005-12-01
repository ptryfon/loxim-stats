#include "ErrorConsole.h"
#include "Errors.h"

using namespace Errors;

int main(int argc, char *argv[])
{
	ErrorConsole *ec = new ErrorConsole;

//	ec->init();
	*ec << "dupa\n";
	*ec << (EBadFile | ErrStore);
	*ec << (ENoFile);
}

#include "ErrorConsole.h"
#include "Errors.h"

using namespace Errors;

int main(int argc, char *argv[])
{
	ErrorConsole *ec = new ErrorConsole("Config");

	ec->init(1);
	*ec << "dupa\n";
	*ec << (EBadFile | ErrStore);
	*ec << (ENoFile);
}

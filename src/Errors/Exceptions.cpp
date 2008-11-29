#include <Errors/Exceptions.h>
#include <Errors/Errors.h>
#include <sstream>
#include <exception>

using namespace Errors;
using namespace std;

Errors::LoximException::LoximException(int err) : runtime_error(construct_message(err)),
	err(err)
{
}

string Errors::LoximException::construct_message(int err)
{
	stringstream ss;
	ss << "Loxim error " << err << "(" << SBQLstrerror(err) << ")";
	return ss.str();
}

int Errors::LoximException::get_error() const {
	return err;
}

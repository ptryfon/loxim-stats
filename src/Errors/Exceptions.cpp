#include <Errors/Exceptions.h>
#include <Errors/Errors.h>
#include <sstream>
#include <exception>
#include <string>
#include <Util/smartptr.h>
#include <Util/Backtrace.h>
#include <vector>
#include <sstream>

using namespace Errors;
using namespace std;
using namespace _smptr;

Errors::LoximException::LoximException(int err) : runtime_error(construct_message(err)),
	err(err)
{
	#ifndef NDEBUG
	bt = Util::get_backtrace(0);	
	#endif
}


string Errors::LoximException::construct_message(int err)
{
	stringstream ss;
	ss << "Loxim error " << err << "(" << SBQLstrerror(err) << ")";
	return ss.str();
}

int Errors::LoximException::get_error() const
{
	return err;
}

const vector<string> &Errors::LoximException::get_bt() const
{
	return bt;
}

string Errors::LoximException::to_string() const
{
	stringstream ss;
	ss << (*this);
	return ss.str();
}

Errors::LoximException::~LoximException() throw() {};


ostream &Errors::operator<<(ostream &stream, const LoximException &ex)
{
	stream << "LoximException " << ex.get_error() << " (" << SBQLstrerror(ex.get_error()) << ")" << endl;
	stream << "Backtrace:" << endl;
	for (vector<string>::const_iterator i = ex.get_bt().begin(); i != ex.get_bt().end(); ++i)
		stream << *i << endl;
	stream << "End of backtrace" << endl;
	return stream;
}

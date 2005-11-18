
#include <sstream>
#include "Result.h"

/** 
 * Result
 */
void Result::toStream(ostream& os) const
{
	os << "Result";
}

ostream& operator<<(ostream& os, Result& result) {
	//result.toStream(os);
	os << "operator";
	return os;
}


/**
 * ResultBag
 */


/**
 * ResultReference
 */
ResultReference::ResultReference(int val)
{
	value = val;
}

string ResultReference::toString()
{
	ostringstream stream;
	stream << value;
	return stream.str();
}


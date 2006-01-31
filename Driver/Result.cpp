
#include <sstream>
#include "Result.h"

namespace Driver {

ostream& operator<<(ostream& os, Result& result) {
	result.toStream(os);
	return os;
}


void   ResultCollection::toStream(ostream& os) const 
{
	os << "{ ";  
	for(unsigned int i=0; i<col.size(); i++) 
	{
		os << *(col.at(i)) << " ";
	} 
	os << "} ";
}


void   ResultStruct::toStream(ostream& os) const 
{
	os << "< ";  
	for(unsigned int i=0; i<col.size(); i++) 
	{
		os << *(col.at(i)) << " ";
	} 
	os << "> ";
}


/**
 * ResultReference
 */

/*string ResultReference::toString()
{
	ostringstream stream;
	stream << value;
	return stream.str();
}
*/

} // namespace

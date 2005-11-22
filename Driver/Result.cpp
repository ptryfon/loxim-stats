
#include <sstream>
#include "Result.h"


ostream& operator<<(ostream& os, Result& result) {
	result.toStream(os);
	return os;
}


void   ResultBag::toStream(ostream& os) const 
{
	os << "{ ";  
	for(unsigned int i=0; i<bag.size(); i++) 
	{
		os << *(bag.at(i)) << " ";
	} 
	os << "} ";
}		

void   ResultStruct::toStream(ostream& os) const 
{
	os << "{ ";  
	for(unsigned int i=0; i<str.size(); i++) 
	{
		os << *(str.at(i)) << " ";
	} 
	os << "} ";
}		


void   ResultSequence::toStream(ostream& os) const 
{
	os << "{ ";  
	for(unsigned int i=0; i<seq.size(); i++) 
	{
		os << *(seq.at(i)) << " ";
	} 
	os << "} ";
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

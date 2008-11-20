#include <protocol/packages/data/DataPart.h>

using namespace protocol;


bool DataPart::equal(DataPart* p1,DataPart* p2)
{
	if (p1==p2)
		return true;
	
	if ((!p1)||(!p2))
		return false;
		
	return p1->equals(p2);
}

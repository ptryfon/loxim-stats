#include "DBLogicalID.h"

using namespace std;

namespace Store
{
	DBLogicalID::DBLogicalID(int v)
	{
		value = v;
	};
	
	PhysicalID* DBLogicalID::getPhysicalID()
	{
		return 0;
	};

	void DBLogicalID::toByteArray(unsigned char** lid, int* length)
	{
	};

	string DBLogicalID::toString() const
	{
	   char cs[100];
		sprintf(cs, "%i", value);
		string s(cs);
		return s;
	};

	bool DBLogicalID::operator==(LogicalID& lid)
	{
		return false;
	};
}


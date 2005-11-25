#include "DBLogicalID.h"

using namespace std;

namespace Store
{
	PhysicalID* DBLogicalID::getPhysicalID()
	{
		return 0;
	};

	void DBLogicalID::toByteArray(unsigned char** lid, int* length)
	{
	};

	string DBLogicalID::toString()
	{
	   char cs[100];
		sprintf(cs, "%ld", value);
		string s(cs);
		return s;
	};

	bool DBLogicalID::operator==(LogicalID& lid)
	{
		return false;
	};
}


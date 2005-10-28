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
		return "123456789";
	};

	bool DBLogicalID::operator==(LogicalID& lid)
	{
		return false;
	};
}


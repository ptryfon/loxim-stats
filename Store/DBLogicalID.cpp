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
		ostringstream str;
		str << value;
		return str.str();
	};
	
	long long DBLogicalID::toLongLong() const
	{
		return static_cast<long long>(value);
	};

	bool DBLogicalID::operator==(LogicalID& lid)
	{
		return (this->toLongLong() == lid.toLongLong());
	};
}


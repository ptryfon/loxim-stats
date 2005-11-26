#include "DBLogicalID.h"

using namespace std;

namespace Store
{
	DBLogicalID::DBLogicalID(unsigned int v)
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
	
	unsigned int DBLogicalID::toInteger() const
	{
		return static_cast<unsigned int>(value);
	};

	bool DBLogicalID::operator==(LogicalID& lid)
	{
		return (this->toInteger() == lid.toInteger());
	};

	DBLogicalID& DBLogicalID::operator=(const LogicalID& lid)
	{
		value = lid.toInteger();;
		return *this;
	};
}


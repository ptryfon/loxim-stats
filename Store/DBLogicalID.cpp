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

	Serialized DBLogicalID::serialize() const
	{
		Serialized s(sizeof(unsigned int));
		*(reinterpret_cast<unsigned int*>(s.bytes)) = value;
		return s;
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


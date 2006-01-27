#include "DBLogicalID.h"

using namespace std;

namespace Store
{
	DBLogicalID::DBLogicalID()
	{
		value = 0;
	};

	DBLogicalID::DBLogicalID(unsigned int v)
	{
		value = v;
	};
	
	DBPhysicalID* DBLogicalID::getPhysicalID()
	{
		return StoreManager::theStore->getPhysicalID(this);
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
		cout << "l 1\n";
		Serialized s(sizeof(unsigned int));
		cout << "l 2\n";
		*(reinterpret_cast<unsigned int*>(s.bytes)) = value;
		cout << "l 3\n";
		return s;
	};

	int DBLogicalID::deserialize(unsigned char* bytes, DBLogicalID*& lid)
	{
		lid = new DBLogicalID(*(reinterpret_cast<unsigned int*>(bytes)));
		return sizeof(unsigned int);
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


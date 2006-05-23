#include "DBLogicalID.h"

using namespace std;

namespace Store
{
	DBLogicalID::DBLogicalID()
	{
		server = "";
		value = 0;
	};

	DBLogicalID::DBLogicalID(unsigned int v)
	{
		server = "";
		value = v;
	};

	DBLogicalID::DBLogicalID(const LogicalID& lid)
	{
		server = lid.getServer();
		value = lid.toInteger();
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
	
	string DBLogicalID::getServer() const
	{
		return server;
	};
	
	void DBLogicalID::setServer(string s)
	{
		server = s;
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

	int DBLogicalID::deserialize(unsigned char* bytes, DBLogicalID*& lid)
	{
		lid = new DBLogicalID(*(reinterpret_cast<unsigned int*>(bytes)));
		return sizeof(unsigned int);
	};
	
	LogicalID* DBLogicalID::clone() const
	{
		return new DBLogicalID((LogicalID&)(*this));
	};

	bool DBLogicalID::operator==(LogicalID& lid)
	{
		return (this->toInteger() == lid.toInteger() && this->getServer() == lid.getServer());
	};

	DBLogicalID& DBLogicalID::operator=(const LogicalID& lid)
	{
		value = lid.toInteger();
		server = lid.getServer();
		return *this;
	};
}


#include <Store/DBLogicalID.h>

using namespace std;

namespace Store
{
	DBLogicalID::DBLogicalID()
	{
		server = "";
		value = 0;
		remoteID = NULL;
		directParentID = NULL;
	};

	DBLogicalID::DBLogicalID(unsigned int v)
	{
		server = "";
		value = v;
		remoteID = NULL;
		directParentID = NULL;
	};

	DBLogicalID::DBLogicalID(const LogicalID& lid)
	{
		server = lid.getServer();
		value = lid.toInteger();
		remoteID = lid.getRemoteID();
		if (remoteID != NULL) {
			remoteID = new DBLogicalID(*lid.getRemoteID());
		}
		directParentID = lid.getDirectParent();
	};

	DBLogicalID::~DBLogicalID()
	{
		if (remoteID != NULL) {
			delete remoteID;
		}
	}

	DBPhysicalID* DBLogicalID::getPhysicalID(TransactionID* tid)
	{
		return StoreManager::theStore->getPhysicalID(tid, this);
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

	int DBLogicalID::getPort() const
	{
		return port;
	};

	void DBLogicalID::setPort(int p)
	{
		port = p;
	};

	LogicalID* DBLogicalID::getRemoteID()  const
	{
		return remoteID;
	}

	void DBLogicalID::setRemoteID(LogicalID* remoteID)
	{
		this->remoteID = remoteID;
	}

	LogicalID* DBLogicalID::getDirectParent() const
	{
		return directParentID;
	}

	void DBLogicalID::setDirectParent(LogicalID* parentID)
	{
		this->directParentID = parentID;
	}


	string DBLogicalID::getParentRoot() const {
		return parentRoot;
	}

	void DBLogicalID::setParentRoot(string parentRoot) {
		this->parentRoot = parentRoot;
	}

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


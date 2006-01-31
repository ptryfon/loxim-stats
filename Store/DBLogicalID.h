#ifndef __STORE_DBLOGICALID_H__
#define __STORE_DBLOGICALID_H__

#include "Store.h"
#include <sstream>
#include "DBStoreManager.h"

using namespace std;

namespace Store
{
	class DBLogicalID : public LogicalID
	{
	public:
		DBLogicalID();
		DBLogicalID(unsigned int value);
		DBLogicalID(const LogicalID& lid);

		virtual DBPhysicalID* getPhysicalID();
		virtual void toByteArray(unsigned char** lid, int* length);
		virtual string toString() const;
		virtual unsigned int toInteger() const;
		virtual Serialized serialize() const;
		static int deserialize(unsigned char* bytes, DBLogicalID*& lid);
		virtual LogicalID* clone() const;

		virtual bool operator==(LogicalID& lid);
		virtual DBLogicalID& operator=(const LogicalID& lid);

		unsigned int value;
	private:
	};
}

#endif


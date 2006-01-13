#ifndef __STORE_DBLOGICALID_H__
#define __STORE_DBLOGICALID_H__

#include "Store.h"
#include <sstream>

using namespace std;

namespace Store
{
	class DBLogicalID : public LogicalID
	{
	public:
		DBLogicalID() {};
		DBLogicalID(unsigned int value);

		virtual PhysicalID* getPhysicalID();
		virtual void toByteArray(unsigned char** lid, int* length);
		virtual string toString() const;
		virtual unsigned int toInteger() const;
		virtual Serialized serialize() const;

		virtual bool operator==(LogicalID& lid);
		virtual DBLogicalID& operator=(const LogicalID& lid);
	private:
		unsigned int value;
	};
}

#endif


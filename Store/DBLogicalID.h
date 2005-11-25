#ifndef __STORE_DBLOGICALID_H__
#define __STORE_DBLOGICALID_H__

#include "Store.h"

using namespace std;

namespace Store
{
	class DBLogicalID : public LogicalID
	{
	public:
		DBLogicalID() {};
		DBLogicalID(int value);

		virtual PhysicalID* getPhysicalID();
		virtual void toByteArray(unsigned char** lid, int* length);
		virtual string toString() const;

		virtual bool operator==(LogicalID& lid);
	private:
		int value;
	};
}

#endif


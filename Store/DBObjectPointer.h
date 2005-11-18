#ifndef __STORE_DBOBJECTPOINTER_H__
#define __STORE_DBOBJECTPOINTER_H__

#include "Store.h"

using namespace std;

namespace Store
{
	class DBObjectPointer : public ObjectPointer
	{
	public:
		DBObjectPointer() {};

		virtual LogicalID* getLogicalID();
		virtual string getName();
		virtual AccessMode getMode();
		virtual DataValue* getValue();
		virtual void setValue(DataValue* val);
		virtual string toString();

		virtual bool operator==(ObjectPointer& dv);
	};
}

#endif


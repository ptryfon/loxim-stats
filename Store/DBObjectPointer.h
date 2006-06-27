#ifndef __STORE_DBOBJECTPOINTER_H__
#define __STORE_DBOBJECTPOINTER_H__

#include "Store.h"

using namespace std;

namespace Store
{
	class DBObjectPointer : public ObjectPointer
	{
	public:
		DBObjectPointer();
		DBObjectPointer(LogicalID*);
		DBObjectPointer(string name, DataValue*);
		DBObjectPointer(string name, DataValue*, LogicalID*);
		~DBObjectPointer();

		virtual LogicalID* getLogicalID() const;
		virtual string getName() const;
		virtual AccessMode getMode() const;
		virtual DataValue* getValue() const;
		virtual void setValue(DataValue* val);
		virtual string toString();
		virtual Serialized serialize() const;
		virtual bool getIsRoot() const;
		virtual void setIsRoot(bool isRoot);
		virtual vector<LogicalID*>* getClasses() const;
		virtual void setClasses(vector<LogicalID*>* value);

		virtual bool operator==(ObjectPointer& dv);
	
	private:
		AccessMode mode;
		string name;
		DataValue* value;
		LogicalID* lid;
		bool isRoot;
	};
}

#endif


#ifndef __STORE_DBDATAVALUE_H__
#define __STORE_DBDATAVALUE_H__

#include "Store.h"

using namespace std;

namespace Store
{
	class DBDataValue : public DataValue
	{
	public:
		DBDataValue() {};

		virtual DataType getType();
		virtual void toByteArray(unsigned char** lid, int* length);
		virtual string toString();

		virtual int getInt();
		virtual double getDouble();
		virtual string getString();
		virtual LogicalID* getPointer();
		virtual vector<ObjectPointer*>* getVector();

		virtual void setInt(int value);
		virtual void setDouble(double value);
		virtual void setString(string value);
		virtual void setPointer(LogicalID* value);
		virtual void setVector(vector<ObjectPointer*>* value);

		virtual bool operator==(DataValue& dv);
	};
}

#endif


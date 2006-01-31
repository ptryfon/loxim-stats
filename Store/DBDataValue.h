#ifndef __STORE_DBDATAVALUE_H__
#define __STORE_DBDATAVALUE_H__

#include "Store.h"
#include "DBLogicalID.h"

using namespace std;

namespace Store
{
	class DBDataValue : public DataValue
	{
	private:
		typedef //struct complex_value {
			//DataType type;
			union {
				int* int_value;
				double* double_value;
				string* string_value;
				LogicalID* pointer_value;
				vector<LogicalID*>* vector_value;
//			} v;
		} complex_value;

	public:
		DBDataValue();
		DBDataValue(int value);
		DBDataValue(double value);
		DBDataValue(string value);
		DBDataValue(LogicalID* value);
		DBDataValue(vector<LogicalID*>* value);
		DBDataValue(const DataValue& value);
		~DBDataValue();

		virtual DataType getType() const;
		virtual string toString();
		virtual Serialized serialize() const;
		static int deserialize(unsigned char* bytes, DBDataValue*& value);
		virtual DataValue* clone() const;

		virtual int getInt() const;
		virtual double getDouble() const;
		virtual string getString() const;
		virtual LogicalID* getPointer() const;
		virtual vector<LogicalID*>* getVector() const;

		virtual void setInt(int value);
		virtual void setDouble(double value);
		virtual void setString(string value);
		virtual void setPointer(LogicalID* value);
		virtual void setVector(vector<LogicalID*>* value);

		virtual bool operator==(DataValue& dv);
		virtual DataValue& operator=(const int& val);
		virtual DataValue& operator=(const double& val);
		virtual DataValue& operator=(const string& val);
		virtual DataValue& operator=(const LogicalID& val);
		virtual DataValue& operator=(const vector<LogicalID*>& val);
		
	private:
		DataType type;
		complex_value value;

		void p_init();
		void p_clearPtr();
		void p_destroyVal();
	};
}

#endif


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
				vector<ObjectPointer*>* vector_value;
//			} v;
		} complex_value;

	public:
		DBDataValue();
		DBDataValue(int value);
		DBDataValue(double value);
		DBDataValue(string value);
		DBDataValue(unsigned char* fullByteArray);
		~DBDataValue();

		virtual DataType getType();
		virtual void toByteArray(unsigned char** buff, int* length);
		virtual void toFullByteArray(unsigned char** buff, int* length);
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
		
	private:
		DataType type;
		complex_value value;

		void p_init();
		void p_clearPtr();
		void p_destroyVal();
	};
}

#endif


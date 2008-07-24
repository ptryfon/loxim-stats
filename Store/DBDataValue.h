#ifndef __STORE_DBDATAVALUE_H__
#define __STORE_DBDATAVALUE_H__

#include "Store.h"
#include "DBLogicalID.h"

using namespace std;
using namespace TManager;

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

	static void cloneSetOfLid(SetOfLids* fromSet, SetOfLids*& toSet) ;
	static void deleteSetOfLid(SetOfLids* toDel);

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
		virtual ExtendedType getSubtype() const;
		virtual void setSubtype(ExtendedType subtype);
		virtual SetOfLids* getClassMarks() const;
		virtual void setClassMarks(SetOfLids* classMarks);
		virtual void addClassMarks(SetOfLids* toAdd);
		virtual void removeClassMark(LogicalID* lid);
		virtual void removeClassMarks(SetOfLids* toDel);
		virtual void addClassMark(LogicalID* classMark);
		virtual SetOfLids* getSubclasses() const;
		virtual void addSubclass(LogicalID* subclass);
		virtual void removeSubclass(LogicalID* lid);
		virtual void setSubclasses(SetOfLids* subclasses);
		virtual void addSubclasses(SetOfLids* toAdd);
		virtual string toString();
		virtual Serialized serialize() const;
		static int deserialize(TransactionID* tid, unsigned char* bytes, DBDataValue*& value, bool AutoRemove=false);
		static void deserializeSetOfLids(unsigned char*& curpos, SetOfLids*& lids);
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
		ExtendedType subtype;

		//this two vector type properties may be consider as usefull for role.

		/** Object class LogicalID or 0 if this object has no class.
		 * If object is a class it contains extended classes. */
		SetOfLids* classMarks;

		/** If object is a class it contains lids to all subclasses.
		 * NULL if object is not a class. */
		SetOfLids* subclasses;

		complex_value value;

		void p_init();
		void p_clearPtr();
		void p_destroyVal();
	};
}

#endif


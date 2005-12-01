#include "DBDataValue.h"

using namespace std;

namespace Store
{
	DBDataValue::DBDataValue()
	{
		p_clearPtr();
		type = Store::Integer;
		value.int_value     = new int(0);
	};
	
	DBDataValue::DBDataValue(int val)
	{
		DBDataValue();
		type = Store::Integer;
		value.int_value = new int(val);
	};

	DBDataValue::DBDataValue(double val)
	{
		DBDataValue();
		type = Store::Double;
		value.double_value = new double(val);
	};

	DBDataValue::DBDataValue(string val)
	{
		DBDataValue();
		type = Store::String;
		value.string_value = new string(val);
	};
	
	DBDataValue::~DBDataValue()
	{
		p_destroyVal();
	};
	 
	DataType DBDataValue::getType()
	{
		return type;
	};

	void DBDataValue::toByteArray(unsigned char** lid, int* length)
	{
	};

	string DBDataValue::toString()
	{
		return "xx";
	};

	int DBDataValue::getInt()
	{
		if( type == Store::Integer )
			return *(value.int_value);
		return 0;    	
	};

	double DBDataValue::getDouble()
	{
		if( type == Store::Double )
			return *(value.double_value);
		return 0;
	};

	string DBDataValue::getString()
	{
		if( type == Store::String )
			return *(value.string_value);
		return "";
	};

	LogicalID* DBDataValue::getPointer()
	{
		if( type == Store::Pointer )
			return value.pointer_value;
		return NULL;
	};

	vector<ObjectPointer*>* DBDataValue::getVector()
	{
		if( type == Store::Vector )
			return value.vector_value;
		return NULL;
	};

	void DBDataValue::setInt(int val)
	{
		p_destroyVal();
		type = Store::Integer;
		value.int_value = new int(val);
	};

	void DBDataValue::setDouble(double val)
	{
		p_destroyVal();
		type = Store::Double;
		value.double_value = new double(val);
	};

	void DBDataValue::setString(string val)
	{
		p_destroyVal();
		type = Store::String;
		value.string_value = new string(val);
	};

	void DBDataValue::setPointer(LogicalID* val)
	{
		p_destroyVal();
		type = Store::Pointer;
		value.pointer_value = val;
	};

	void DBDataValue::setVector(vector<ObjectPointer*>* val)
	{
		p_destroyVal();
		type = Store::Vector;
		value.vector_value = val;
	};

	bool DBDataValue::operator==(DataValue& dv)
	{
		if( this->getType() != dv.getType() )
			return false;
		switch( this->getType() ) {
			case Store::Integer:
				return (this->getInt() == dv.getInt());
			case Store::Double:
				return (this->getDouble() == dv.getDouble());
			case Store::String:
				return (this->getString() == dv.getString());
			case Store::Pointer:
				return (this->getPointer() == dv.getPointer());
			case Store::Vector:
				return (this->getVector() == dv.getVector());
		}
		return false;
	};
	
	void DBDataValue::p_destroyVal()
	{
		switch(type) {
			case Store::Integer:
				if(value.int_value) delete value.int_value; break;
			case Store::Double:
				if(value.double_value) delete value.double_value; break;
			case Store::String:
				if(value.string_value) delete value.string_value; break;
			case Store::Pointer:
//				if(value.pointer_value) delete value.pointer_value;
				break;
			case Store::Vector:
//				if(value.vector_value) delete value.vector_value;
				break;
		}
		p_clearPtr();
	};
	
	void DBDataValue::p_clearPtr()
	{
		value.int_value = NULL;
		//could clear all fields bu this is an union (4bytes?)
	}
}


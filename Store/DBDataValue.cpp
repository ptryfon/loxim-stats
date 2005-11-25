#include "DBDataValue.h"

using namespace std;

namespace Store
{
	DataType DBDataValue::getType()
	{
		return this->type;
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
	    if (this->type == Store::Integer) 
	
		return this->value.v.int_value;
	    return 0;		
	    	
	};

	double DBDataValue::getDouble()
	{
	
	    if ( this->type == Store::Double ) 
		return this->value.v.double_value;
	    return 0;	
		
	};

	string DBDataValue::getString()
	{
		return 0;    
	};

	LogicalID* DBDataValue::getPointer()
	{
	    if ( this->type == Store::Pointer ) 
		return this->value.v.pointer_value;
	    return 0;	
	};

	vector<ObjectPointer*>* DBDataValue::getVector()
	{
	    if ( this->type == Store::Vector ) 
		return this->value.v.vector_value;
	    return 0;	
	};

	void DBDataValue::setInt(int value)
	{
	};

	void DBDataValue::setDouble(double value)
	{
	};

	void DBDataValue::setString(string value)
	{
	};

	void DBDataValue::setPointer(LogicalID* value)
	{
	};

	void DBDataValue::setVector(vector<ObjectPointer*>* value)
	{
	};

	bool DBDataValue::operator==(DataValue& dv)
	{
		return false;
	};
}


#include "DBDataValue.h"

using namespace std;

namespace Store
{
	DataValue::DataType DBDataValue::getType()
	{
		return DataType::Integer;
	};

	void DBDataValue::toByteArray(unsigned char** lid, int* length)
	{
	};

	string DBDataValue::toString()
	{
		return "5";
	};

	int DBDataValue::getInt()
	{
		return 5;
	};

	double DBDataValue::getDouble()
	{
		return 5.0;
	};

	string DBDataValue::getString()
	{
		return "5";
	};

	LogicalID* DBDataValue::getPointer()
	{
		return 0;
	};

	vector<ObjectPointer*>* DBDataValue::getVector()
	{
		return new vector<ObjectPointer*>(0);
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


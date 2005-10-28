#include "DBObjectPointer.h"
#include "DBLogicalID.h"
#include "DBDataValue.h"

using namespace std;

namespace Store
{
	LogicalID* DBObjectPointer::getLogicalID()
	{
		return new DBLogicalID();
	};

	string DBObjectPointer::getName()
	{
		return "empty";
	};

	ObjectPointer::AccessMode DBObjectPointer::getMode()
	{
		return ObjectPointer::Read;
	};

	DataValue* DBObjectPointer::getValue()
	{
		return new DBDataValue();
	};

	void DBObjectPointer::setValue(DataValue* val)
	{
	};

	string DBObjectPointer::toString()
	{
		return this->getLogicalID()->toString() + " " + this->getValue()->toString();
	};

	bool DBObjectPointer::operator==(ObjectPointer& dv)
	{
		return false;
	};
}


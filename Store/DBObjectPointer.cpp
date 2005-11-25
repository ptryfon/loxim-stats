#include "DBObjectPointer.h"
#include "DBLogicalID.h"
#include "DBDataValue.h"

using namespace std;

namespace Store
{
	DBObjectPointer::DBObjectPointer(string nam, DataValue* dv)
	{
		name = nam;
		value = dv;
	};
	
	LogicalID* DBObjectPointer::getLogicalID()
	{
		return this->lid;
	};

	string DBObjectPointer::getName()
	{
		return this->name;
	};

	AccessMode DBObjectPointer::getMode()
	{
		return this->mode;
	};

	DataValue* DBObjectPointer::getValue()
	{
		return this->value;
	};

	void DBObjectPointer::setValue(DataValue* val)
	{
	};

	string DBObjectPointer::toString()
	{
		return "<" + this->getLogicalID()->toString() + ", " + this->getName() + ", " + this->getValue()->toString() + ">";
	};

	bool DBObjectPointer::operator==(ObjectPointer& dv)
	{
		return false;
	};
}


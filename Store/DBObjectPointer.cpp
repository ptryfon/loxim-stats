#include <iostream>
#include "DBObjectPointer.h"
#include "DBLogicalID.h"
#include "DBDataValue.h"

using namespace std;

namespace Store
{
	DBObjectPointer::DBObjectPointer()
	{
		lid = NULL;
		name = "NoName";
		value = NULL;
	};
	
	DBObjectPointer::DBObjectPointer(string p_name, DataValue* dv)
	{
		lid = NULL;
		name = p_name;
		value = dv;
	};

	DBObjectPointer::DBObjectPointer(string p_name, DataValue* dv, LogicalID* ld)
	{
		lid = ld;
		name = p_name;
		value = dv;
	};
	
	DBObjectPointer::~DBObjectPointer()
	{
//		cout << "object destroyd\n";
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
		this->value = val; //pointer copy?
	};

	string DBObjectPointer::toString()
	{
		return "<" + 
			(this->getLogicalID() ?
				this->getLogicalID()->toString() : "NoLogicalID") +
			", \"" + this->getName() + "\", " + 
			(this->getValue() ?
				this->getValue()->toString() : "NoValue") + ">";
	};

	bool DBObjectPointer::operator==(ObjectPointer& dv)
	{
		return false;
	};
}


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

	DBObjectPointer::DBObjectPointer(LogicalID* p_lid)
	{
		lid = p_lid;
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
		if(lid) delete lid;
		if(value) delete value;
	};
	
	LogicalID* DBObjectPointer::getLogicalID() const
	{
		return this->lid;
	};

	string DBObjectPointer::getName() const
	{
		return this->name;
	};

	AccessMode DBObjectPointer::getMode() const
	{
		return this->mode;
	};

	DataValue* DBObjectPointer::getValue() const
	{
		return this->value;
	};

	void DBObjectPointer::setValue(DataValue* val)
	{ // czy to nie jest ta funkcja ktora mialaby wywlywac
	  // deleteObject + createObject hmmmmm
		DBStoreManager::theStore->replaceDV(this, val); 
		this->value = val; //pointer copy
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

	Serialized DBObjectPointer::serialize() const
	{
		Serialized s;
		//LogicalID
		cout << "s 1\n";
		s += *(getLogicalID());
		//Random
		cout << "s 2\n";
		s += static_cast<int>(0x78563412);//((rand()%0x100) << 24 +
			//(rand()%0x100) << 16 + (rand()%0x100) << 8 + (rand()%0x100));
		//Name
		cout << "s 3\n";
		s += getName();
		//Value
		cout << "s 4\n";
		s += *(getValue());
		
		cout << "s 6\n";
		return s;
	};

	bool DBObjectPointer::operator==(ObjectPointer& dv)
	{
		return (this->getLogicalID() == dv.getLogicalID());
	};
}

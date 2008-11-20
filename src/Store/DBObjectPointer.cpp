#include <iostream>
#include <Store/DBObjectPointer.h>
#include <Store/DBLogicalID.h>
#include <Store/DBDataValue.h>
#include <map>

//#define DBOP_DEBUG

using namespace std;

namespace Store
{
	map<LogicalID*, vector<LogicalID*>*> classes;

	DBObjectPointer::DBObjectPointer()
	{
#ifdef DBOP_DEBUG
		cout << "DBObjectPointer::Constructor()\n";
#endif
		lid = NULL;
		name = "NoName";
		value = NULL;
	};

	DBObjectPointer::DBObjectPointer(LogicalID* p_lid)
	{
#ifdef DBOP_DEBUG
		cout << "DBObjectPointer::Constructor(LID*)\n";
#endif
		lid = p_lid->clone();
		name = "NoName";
		value = NULL;
	};

	
	DBObjectPointer::DBObjectPointer(string p_name, DataValue* dv)
	{
#ifdef DBOP_DEBUG
		cout << "DBObjectPointer::Constructor(string, DV*)\n";
#endif
		lid = NULL;
		name = p_name;
		value = dv->clone();
	};

	DBObjectPointer::DBObjectPointer(string p_name, DataValue* dv, LogicalID* p_lid)
	{
#ifdef DBOP_DEBUG
		cout << "DBObjectPointer::Constructor(string, DV*, LID*)\n";
#endif
		lid = p_lid->clone();
		name = p_name;
		value = dv->clone();
	};
	
	DBObjectPointer::~DBObjectPointer()
	{
#ifdef DBOP_DEBUG
		cout << "DBObjectPointer::Destructor\n";
#endif
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
		cout << "DBObjectPointer::NIELEGALNA FUNKCJA !!!\n";
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
				this->getValue()->toString() : "NoValue") + ", " +
			(this->isRoot ? "root" : "chld") + ">";
	};

	Serialized DBObjectPointer::serialize() const
	{
#ifdef DBOP_DEBUG
		cout << "DBObjectPointer::serialize\n";
#endif
		Serialized s;
		//LogicalID
		s += *(getLogicalID());
		//IsRoot
		s += (int) (getIsRoot() ? 1 : 0);
		//Random
		s += static_cast<int>(0x78563412);//((rand()%0x100) << 24 +
			//(rand()%0x100) << 16 + (rand()%0x100) << 8 + (rand()%0x100));
		//Name
		s += getName();
		//Value
		s += *(getValue());
		
		return s;
	};

	bool DBObjectPointer::operator==(ObjectPointer& dv)
	{
		return (this->getLogicalID() == dv.getLogicalID());
	};
	
	bool DBObjectPointer::getIsRoot() const
	{
		return isRoot;
	};
	
	void DBObjectPointer::setIsRoot(bool isRoot)
	{
		this->isRoot = isRoot;
	};
	
	string DBObjectPointer::getParentRoot() const {
			return parentRoot;
	}
		
	void DBObjectPointer::setParentRoot(string parentRoot) {
		this->parentRoot = parentRoot;
	}
	
	vector<LogicalID*>* DBObjectPointer::getClasses() const
	{
		return 0;
	};
	
	void DBObjectPointer::setClasses(vector<LogicalID*>*)
	{
	};
}

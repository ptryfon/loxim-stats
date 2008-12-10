#include "OuterSchema.h"
#include "InterfaceMaps.h"
#include "TransactionManager/Transaction.h"
#include "Store/DBLogicalID.h"

using namespace Schemas;

void OuterSchema::addName(string name, int crudFlags)
{
	if (crudFlags == 0)
		return; //name with no access is like no name at all
	
	m_namesInSchema[name] = crudFlags;	
}

int OuterSchema::getCrudForName(string name) const
{
	TNameToAccess::const_iterator it = m_namesInSchema.find(name);
	if (it != m_namesInSchema.end())
	{
		int crud = (*it).second;
		return crud;
	}
	return 0;
}

void OuterSchema::addName(string name, EntityType t, bool canCreate, bool canRead, bool canUpdate, bool canDelete)
{   //bool values ensure CRUD int validity
	int crud = 0;
	if (canCreate) crud += CREATE;
	if (canRead) crud += READ;
	if (canUpdate) crud += UPDATE;
	if (canDelete) crud += DELETE;
	addName(name, crud);
	if (t == INTERFACE)
	{
		bool found;
		string objectName = InterfaceMaps::Instance().getObjectNameForInterface(name, found);
		if (found)
			addName(objectName, crud); //TODO - CRUD for interface objects should be separate
	}
}

void OuterSchema::removeName(string name)
{
	m_namesInSchema.erase(name);
}

bool OuterSchema::nameVisible(string name) const
{
	return (getCrudForName(name) != 0);
}

bool OuterSchema::canCreate(string name) const
{
	return (getCrudForName(name) & CREATE);
}

bool OuterSchema::canRead(string name) const
{
	return (getCrudForName(name) & READ);
}

bool OuterSchema::canUpdate(string name) const
{
	return (getCrudForName(name) & UPDATE);
}

bool OuterSchema::canDelete(string name) const
{
	return (getCrudForName(name) & DELETE);
}

void OuterSchema::print(ErrorConsole *ec) const
{
	ec->printf("OuterSchema %s\n", m_name.c_str());
	TNameToAccess::const_iterator it;
	for (it = m_namesInSchema.begin(); it != m_namesInSchema.end(); ++it)
	{
		string name = (*it).first;
		string crudString = "";
		if (canCreate(name)) crudString += "CREATE ";
		if (canRead(name)) crudString += "READ ";
		if (canUpdate(name)) crudString += "UPDATE ";
		if (canDelete(name)) crudString += "DELETE ";
		ec->printf("\t %s %s \n", name.c_str(), crudString.c_str());	
	}
}

string OuterSchema::toString(TManager::Transaction *tr) const
{
	string out = "schema ";
	out += m_name;
	out += "\n{\n";
	bool exists;
	TNameToAccess::const_iterator it;
	for (it = m_namesInSchema.begin(); it != m_namesInSchema.end(); ++it)
	{
		string name = (*it).first;
		int crud = (*it).second;
		
		Schema s;
		Schema *sp = &s;
		int t;
		int errcode = Schema::getCIVObject(name, tr, exists, t, sp);
		
		out += sp->toSchemaString();
		out += " ";
		out += getCrudString(crud);
		out += "\n";
	}
	out += "}\n";
	return out;
}


int OuterSchema::fromLogicalID(LogicalID *lid, TManager::Transaction *tr, OuterSchema *&s)
{
	if (!s) s = new OuterSchema();
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(lid, Store::Read, optr, false);
	if (errcode != 0)
		return errcode;
	DataValue* dv = optr->getValue();
	s->setName(optr->getName());
	if(dv->getSubtype() != Store::StoreSchema) return -1;
	
	TNameToAccess aps;
	vector<LogicalID*>* lidVec = dv->getVector();
	vector<LogicalID*>::iterator it;
	for (it = lidVec->begin(); it!=lidVec->end(); ++it) 
	{
		LogicalID* currentLid = *it;
		ObjectPointer *optrInside;
		errcode = tr->getObjectPointer (currentLid, Store::Read, optrInside, false);
		if (errcode != 0)
			return errcode;		
		DataValue *dvInside = optrInside->getValue();
		if (optrInside->getName() == QE_FIELD_BIND_NAME) 
		{
			string apName = dvInside->getString();
			//cout << "AP Name: " << apName << endl;
			aps[apName] = defaultCrud;	//TODO	  
		}
	}
	s->setAccessPoints(aps);
	return 0;
}

/***************
* OuterSchemas *
***************/

OuterSchemas::OuterSchemas()
{
	 ec = new ErrorConsole("OuterSchemas");
}

int OuterSchemas::loadOuterSchemas(TManager::Transaction *tr, TLidsVector *lvec)
{   
	m_outerSchemas.clear();
	int errcode;
	TLidsVector::iterator it;
	for (it = lvec->begin(); it != lvec->end(); ++it)
	{
		LogicalID *schemaLid = *it;
		OuterSchema s;
		OuterSchema *sp = &s;
		errcode = OuterSchema::fromLogicalID(schemaLid, tr, sp);
		if (errcode) return errcode;
		STATE state = OuterSchemaValidator::validate(sp, tr, true);
		if (OuterSchemaValidator::isError(state)) return -1; //TODO
		m_outerSchemas[sp->getName()] = *sp;
	}

	return 0;
}
		
int OuterSchemas::init()
{   
	Transaction *tr;
	int errcode = (TransactionManager::getHandle())->createTransaction(-1, tr);
	if (errcode != 0) 
	{
		return errcode;
	}
	ec->printf("OuterSchemas::init() getting lids..\n");
	vector<LogicalID*>* schemasLids;
	errcode = tr->getSchemasLID(schemasLids);
	if (errcode != 0) 
	{
		ec->printf("OuterSchemas::init() error\n");
		return errcode;
	}
	
	ec->printf("OuterSchemas::init() loading outer schemas..\n");
	errcode = loadOuterSchemas(tr, schemasLids);
	if (errcode != 0) 
	{
		ec->printf("OuterSchemas::loadOuterSchemas() error, %d\n", errcode);
		return errcode;
	}
	errcode = tr->commit();
	if (errcode) 
	{
		return errcode;
	}
	ec->printf("OuterSchemas::init(): loaded %d schemas, returning\n", m_outerSchemas.size());
	return 0;
}

void OuterSchemas::deinit()
{
	clearSchemas(); 
	if (ec) 
	{
		delete ec;
		ec = NULL;
	};
}

void OuterSchemas::addSchema(OuterSchema s)
{
	string name = s.getName();
	m_outerSchemas[name] = s;
	TNameToAccess aps = s.getAccessPoints();
	TNameToAccess::const_iterator it;
	for (it = aps.begin(); it != aps.end(); ++it)
	{
		string apsName = (*it).first;
		TOuterSchemas schemasForName = getAllSchemasUsingName(apsName);
		schemasForName[name] = s;
		m_namesInSchemas[apsName] = schemasForName;
	}
}

void OuterSchemas::removeSchema(string name)
{
	OuterSchema s = m_outerSchemas[name];
	TNameToAccess aps = s.getAccessPoints();
	TNameToAccess::const_iterator it;
	for (it = aps.begin(); it != aps.end(); ++it)
	{
		string apsName = (*it).first;
		TOuterSchemas schemasForName = getAllSchemasUsingName(apsName);
		schemasForName.erase(name);
		m_namesInSchemas[apsName] = schemasForName;
	}
	m_outerSchemas.erase(name);
}

OuterSchema OuterSchemas::getSchema(string name) const
{
	OuterSchema s;
	TOuterSchemas::const_iterator it = m_outerSchemas.find(name);
	if (it != m_outerSchemas.end())
		s = (*it).second;
	return s;
}

TOuterSchemas OuterSchemas::getAllSchemasUsingName(string s) const
{
	TOuterSchemas out;
	TNameInSchemas::const_iterator it = m_namesInSchemas.find(s);
	if (it != m_namesInSchemas.end())
		out = (*it).second;
	return out;
}

string OuterSchemas::toString(TManager::Transaction *tr) const
{	
	string out;
	bool exists;
	int type;
	TOuterSchemas::const_iterator it;
	for (it = m_outerSchemas.begin(); it != m_outerSchemas.end(); ++it)
	{
		OuterSchema s = (*it).second;
		out += s.toString(tr);
		out += "\n";
	}
	return out;
}

void OuterSchemas::debugPrint(TManager::Transaction *tr) const
{
	cout << toString(tr);
}

/***********************
* OuterSchemaValidator *
***********************/
	
STATE OuterSchemaValidator::validate(OuterSchema *&s, TManager::Transaction *tr, bool isNew)
{   
	s->setValidity(OUTERSCHEMA_INVALID_OTHER);
	string outerSchemaName = s->getName();
	if (isNew && OuterSchemas::Instance().hasSchemaName(outerSchemaName))
		return ERROR_NAME_NOT_UNIQUE;
	
	TNameToAccess aps = s->getAccessPoints();
	for (TNameToAccess::iterator it = aps.begin(); it != aps.end(); ++it)
	{
		string name = (*it).first;
		bool exists;
		Schema *sp = NULL;
		int t;
		int errcode = Schema::getCIVObject(name, tr, exists, t, sp);
		if (errcode) return ERROR_TM;		
		if (!exists) return ERROR_NO_NAME;
		if (t == BIND_INTERFACE)
		{
			SchemaValidity v = validateInterfaceBind(name, tr);
			s->setValidity(v);
			if (v != OUTERSCHEMA_VALID)
				return INVALID;
		}
	}

	s->setValidity(OUTERSCHEMA_VALID);
	return VALID;
}

SchemaValidity OuterSchemaValidator::validateInterfaceBind(string interfaceName, TManager::Transaction *tr)
{
	bool found;
	ImplementationInfo i = InterfaceMaps::Instance().getImplementationForInterface(interfaceName, found, false);
	if (!found)
		return OUTERSCHEMA_INVALID_NO_BIND;

	string boundName = i.getName();
	int boundType = i.getType();
	
	bool matches;
	Schema::interfaceMatchesImplementation(interfaceName, boundName, tr, boundType, matches);
	if (!matches)
		return OUTERSCHEMA_INVALID_BAD_BIND;
	
	if (boundType == BIND_INTERFACE)
	{
		return validateInterfaceBind(boundName, tr);
	}

	return OUTERSCHEMA_VALID;
}



#include "OuterSchema.h"
#include "AccessMap.h"
#include "QueryResult.h"
#include "InterfaceMaps.h"
#include "CommonOperations.h"
#include "TransactionManager/Transaction.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include <iostream>
#include <fstream>

using namespace Schemas;
using namespace CommonOperations;

void OuterSchema::addName(string name, int crudFlags)
{
	if (crudFlags == 0)
		return; //name with no access is like no name at all


	bool found;
	string objectName = InterfaceMaps::Instance().getObjectNameForInterface(name, found);
	if (found)
	{
		m_namesInSchema[name] = crudFlags;	
		//m_namesInSchema[objectName] = crudFlags;
	}
	else
	{
		string interfaceName = InterfaceMaps::Instance().getInterfaceNameForObject(name, found);
		if (found)
			m_namesInSchema[interfaceName] = crudFlags;
	}
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
	debug_printf(*ec, "OuterSchema %s\n", m_name.c_str());
	TNameToAccess::const_iterator it;
	for (it = m_namesInSchema.begin(); it != m_namesInSchema.end(); ++it)
	{
		string name = (*it).first;
		string crudString = "";
		if (canCreate(name)) crudString += "CREATE ";
		if (canRead(name)) crudString += "READ ";
		if (canUpdate(name)) crudString += "UPDATE ";
		if (canDelete(name)) crudString += "DELETE ";
		debug_printf(*ec, "\t %s %s \n", name.c_str(), crudString.c_str());
	}
}

string OuterSchema::toString(TManager::Transaction *tr) const
{
	string out = "schema ";
	out += m_name;
	out += " \n{\n";
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
		out += ";\n";
	}
	out += "} \n";
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
	if(dv->getSubtype() != Store::StoreSchema) 
		return (ErrQExecutor | EOtherResExp);;
	
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
			++it;
			if (it==lidVec->end())
				return (ErrQExecutor | EOtherResExp);; //no crud for this one!
			LogicalID* currentCrudLid = *it;
			ObjectPointer *optrCrudInside;
			errcode = tr->getObjectPointer (currentCrudLid, Store::Read, optrCrudInside, false);
			if (errcode != 0)
				return errcode;		
			DataValue *dvCrudInside = optrCrudInside->getValue();
			if (optrCrudInside->getName() != QE_CRUD_BIND_NAME) 
			{
				return (ErrQExecutor | EOtherResExp);; //crud expected!
			}
			else
			{
				int cr = dvCrudInside->getInt();
				s->addName(apName, cr);
			}
		}
	}
	return 0;
}

/***************
* OuterSchemas *
***************/

OuterSchemas::OuterSchemas()
{
	 ec = &ErrorConsole::get_instance(EC_OUTER_SCHEMAS);
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
		STATE state = OuterSchemaValidator::validate(*sp, tr, true);
		if (OuterSchemaValidator::isError(state))
			return (ErrQExecutor | ESchemaIsCriticallyInvalid);
		addSchema(*sp, state);
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
	debug_printf(*ec, "OuterSchemas::init() getting lids..\n");
	vector<LogicalID*>* schemasLids;
	errcode = tr->getSchemasLID(schemasLids);
	if (errcode != 0) 
	{
		debug_printf(*ec, "OuterSchemas::init() error\n");
		return errcode;
	}
	
	debug_printf(*ec, "OuterSchemas::init() loading outer schemas..\n");
	errcode = loadOuterSchemas(tr, schemasLids);
	if (errcode != 0) 
	{
		debug_printf(*ec, "OuterSchemas::loadOuterSchemas() error, %d\n", errcode);
		return errcode;
	}
	errcode = tr->commit();
	if (errcode) 
	{
		return errcode;
	}
	debug_printf(*ec, "OuterSchemas::init(): loaded %d schemas, returning\n", m_outerSchemas.size());
	return 0;
}

void OuterSchemas::deinit()
{
	clearSchemas(); 
	if (ec) 
	{
		ec = NULL;
	};
}

int OuterSchemas::addSchema(LogicalID *schemaLid, TManager::Transaction *tr, int ct)
{
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(schemaLid, Store::Read, optr, false);
	if (errcode) 
	{
		debug_printf(*ec, "[OuterSchemas::addSchema] error in getObjectPointer");
		return errcode;
	}
	debug_printf(*ec, "[OuterSchemas::addSchema] got object pointer\n");
	string schemaName = optr->getName();
	
	bool taken;
	errcode = nameTaken(schemaName, tr, taken);	
	if (errcode) return errcode;
	if ((taken) && (ct == CT_CREATE))
	{ 
		debug_printf(*ec, "[OuterSchemas::addSchema] - cannot create schema %s, name already in use\n", schemaName.c_str());
		return (ErrQExecutor | ENotUniqueSchemaName);
	}
	bool present = hasSchemaName(schemaName);
	if ((!present) && (ct == CT_UPDATE))
	{
		debug_printf(*ec, "[OuterSchemas::addSchema] - cannot update schema %s, no such schema\n", schemaName.c_str());
		return (ErrQExecutor | ENoSchemaFound);
	}
	if (present)
	{
		errcode = removeSchema(schemaName, tr);
		if (errcode) return errcode;
	}

	errcode = tr->addRoot(optr);
	if (errcode != 0) 
	{
		debug_printf(*ec, "[OuterSchemas::addSchema] - error in addRoot");
		return errcode;
	}
	debug_printf(*ec, "[OuterSchemas::addSchema] addRoot processed\n");

	errcode = tr->addSchema(schemaName.c_str(), optr);
	if (errcode != 0) 
	{
		debug_printf(*ec, "[OuterSchemas::addSchema] - error in addSchema");
		return errcode;
	}

	OuterSchema *s = NULL;
	errcode = OuterSchema::fromLogicalID(schemaLid, tr, s);
	if (errcode) return errcode;
	STATE state = OuterSchemaValidator::validate(*s, tr, true);
	debug_printf(*ec, "[OuterSchema::addSchema] validation shows state %d", state);
	if (OuterSchemaValidator::isError(state))
		return (ErrQExecutor | EInvalidSchemaDefinition);
	addSchema(*s, state);
	return 0;
}

int OuterSchemas::removeSchema(string name, TManager::Transaction *tr)
{   
	debug_printf(*ec, "[QE] removing schema %s\n", name.c_str());
	vector<LogicalID*>* lids;
	int errcode = tr->getSchemasLID(name, lids);
	LogicalID *oldLid = lids->at(0);
	ObjectPointer *p;
	errcode = tr->getObjectPointer (oldLid, Store::Write, p, true);
	if (errcode || !p) 
	{
		debug_printf(*ec, "[OuterSchemas::removeSchemas] getObjectPointer returned error\n");
		return errcode;
	}
		
	return removeSchema(name, tr, p);
}

int OuterSchemas::removeSchema(string name, TManager::Transaction *tr, ObjectPointer *p)
{   
	debug_printf(*ec, "[QE] removing schema %s\n", name.c_str());
	int errcode = tr->removeSchema(p);
	if (errcode) return errcode; 
	if (p->getIsRoot())
	{
		errcode = tr->removeRoot(p);
		if (errcode) return errcode;
	}
	errcode = tr->deleteObject(p);
	if (errcode) return errcode;
	removeSchema(name);
	return 0;	
}

void OuterSchemas::addSchema(OuterSchema s, STATE state)
{
	string name = s.getName();
	m_outerSchemas[name] = s;
	m_schemasValidity[name] = state;
	TNameToAccess aps = s.getAccessPoints();
	TNameToAccess::const_iterator it;
	for (it = aps.begin(); it != aps.end(); ++it)
	{
		string apsName = (*it).first;
		set<string> schemasForName = getAllSchemasUsingName(apsName);
		schemasForName.insert(name);
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
		set<string> schemasForName = getAllSchemasUsingName(apsName);
		schemasForName.erase(name);
		m_namesInSchemas[apsName] = schemasForName;
	}
	m_outerSchemas.erase(name);
	m_schemasValidity.erase(name);
}

OuterSchema OuterSchemas::getSchema(string name) const
{
	OuterSchema s;
	TOuterSchemas::const_iterator it = m_outerSchemas.find(name);
	if (it != m_outerSchemas.end())
		s = (*it).second;
	return s;
}

set<string> OuterSchemas::getAllSchemasUsingName(string s) const
{
	set<string> out;
	TNameInSchemas::const_iterator it = m_namesInSchemas.find(s);
	if (it != m_namesInSchemas.end())
		out = (*it).second;
	return out;
}

void OuterSchemas::revalidateAllSchemasUsingName(string s, TManager::Transaction *tr)
{
	set<string> toValidate = getAllSchemasUsingName(s);
	set<string>::iterator it;
	for (it = toValidate.begin(); it != toValidate.end(); ++it)
	{
		string name = (*it);
		OuterSchema sch = m_outerSchemas[name];
		STATE s = OuterSchemaValidator::validate(sch, tr);
		m_schemasValidity[name] = s;
	}
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

STATE OuterSchemas::getValidity(string schemaName) const
{
	TValidityMap::const_iterator it =  m_schemasValidity.find(schemaName);
	if (it != m_schemasValidity.end())
	{
		STATE s = it->second;
		return s;
	}
	return INVALID_OTHER;
}

bool OuterSchemas::isValid(string schemaName) const
{
	STATE s = getValidity(schemaName);
	return (!OuterSchemaValidator::isInvalid(s));
}

int OuterSchemas::exportSchema(string schemaName, TManager::Transaction *tr) const
{
	if (!hasSchemaName(schemaName))
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] no schema by name %s\n", schemaName.c_str());
		return (ErrQExecutor | ENoSchemaFound);
	}
	OuterSchema s = getSchema(schemaName);
	STATE state = getValidity(schemaName);
	if (OuterSchemaValidator::isInvalid(state))
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] schema %s is invalid (%d) and cannot be exported\n", schemaName.c_str(), state);
		return (ErrQExecutor | ESchemaInvalidCannotExport);
	}
	
	string exportedContent = s.toString(tr);
	string fileName = getSchemaFile(schemaName);
	ofstream file;
	file.open(fileName.c_str(), ios::out | ios::trunc);
	if (!file.is_open())
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] could not open file %s\n", fileName.c_str());
		return (ErrQExecutor | ENoFile);
	}
	else
	{
		file << exportedContent;
		file.close();
	}
	return 0;
}

int OuterSchemas::importSchema(string schemaName, string &out) const
{
	if (hasSchemaName(schemaName))
	{
		debug_printf(*ec, "[OuterSchemas::importSchema] schema by name %s already exists (delete first)\n", schemaName.c_str());
		return (ErrQExecutor | ESchemaAlreadyExists);
	}
	string schemaString, line;
	string fileName = getSchemaFile(schemaName);
	ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open())
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] could not open file %s\n", fileName.c_str());
		return (ErrQExecutor | ENoFile);
	}
	else
	{
		while (!file.eof())
		{
			getline(file,line);
			schemaString += line;
		}
		file.close();
	}
	schemaString = "create " + schemaString;
	out = schemaString;
	return 0;
}

/***********************
* OuterSchemaValidator *
***********************/
	
STATE OuterSchemaValidator::validate(const OuterSchema &s, TManager::Transaction *tr, bool beforeAdding)
{   
	STATE validity = VALID;
	string outerSchemaName = s.getName();
	if (beforeAdding && OuterSchemas::Instance().hasSchemaName(outerSchemaName))
		return ERROR_NAME_NOT_UNIQUE;
	
	TNameToAccess aps = s.getAccessPoints();
	for (TNameToAccess::iterator it = aps.begin(); it != aps.end(); ++it)
	{
		string name = (*it).first;
		bool exists;
		Schema *sp = NULL;
		int t;
		int errcode = Schema::getCIVObject(name, tr, exists, t, sp);
		if (errcode) return ERROR_TM;		
		if (!exists) return ERROR_NO_NAME;
		if (t != BIND_INTERFACE) 
			return ERROR_NOT_INTERFACE; //Only interfaces are allowed in schema
		if (!sp->getAssociatedObjectName().empty())
		{	//If interface has object name, it must be bound	   
			STATE interfaceValidity = validateInterfaceBind(name, tr);
			if (isInvalid(interfaceValidity))
				validity = interfaceValidity;
		}
	}

	return validity;
}

STATE OuterSchemaValidator::validateInterfaceBind(string interfaceName, TManager::Transaction *tr)
{
	bool found;
	ImplementationInfo i = InterfaceMaps::Instance().getImplementationForInterface(interfaceName, found, false);
	if (!found)
		return INVALID_NO_BIND;

	string boundName = i.getName();
	int boundType = i.getType();
	
	bool matches;
	Schema::interfaceMatchesImplementation(interfaceName, boundName, tr, boundType, matches);
	if (!matches)
		return INVALID_BAD_BIND;
	
	if (boundType == BIND_INTERFACE)
	{
		return validateInterfaceBind(boundName, tr);
	}

	return VALID;
}



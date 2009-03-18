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
	if(dv->getSubtype() != Store::StoreSchema) return -1;
	
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
			++it;
			if (it==lidVec->end())
				return -1; //no crud for this one!
			LogicalID* currentCrudLid = *it;
			ObjectPointer *optrCrudInside;
			errcode = tr->getObjectPointer (currentCrudLid, Store::Read, optrCrudInside, false);
			if (errcode != 0)
				return errcode;		
			DataValue *dvCrudInside = optrCrudInside->getValue();
			if (optrCrudInside->getName() != QE_CRUD_BIND_NAME) 
			{
				return -1; //crud expected!
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
		STATE state = OuterSchemaValidator::validate(sp, tr, true);
		if (OuterSchemaValidator::isError(state)) return -1; //TODO
		addSchema(*sp);
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
	debug_printf(*ec, "[QE] TNREGSCHEMA got object pointer\n");
	string schemaName = optr->getName();
	
	bool taken;
	errcode = nameTaken(schemaName, tr, taken);	
	if (errcode) return errcode;
	if ((taken) && (ct == CT_CREATE))
	{ 
		debug_printf(*ec, "[QE] TNSCHEMANODE - cannot create schema %s, name already in use\n", schemaName.c_str());
		return -1;
	}
	bool present = hasSchemaName(schemaName);
	if ((!present) && (ct == CT_UPDATE))
	{
		debug_printf(*ec, "[QE] TNSCHEMANODE - cannot update schema %s, no such schema\n", schemaName.c_str());
		return -1;	
	}
	if (present)
	{
		errcode = removeSchema(schemaName, tr);
		if (errcode) return errcode;
	}

	errcode = tr->addRoot(optr);
	if (errcode != 0) 
	{
		debug_printf(*ec, "[QE] TNSCHEMANODE - error in addRoot");
		return errcode;
	}
	debug_printf(*ec, "[QE] TNREGSCHEMA addRoot processed\n");

	errcode = tr->addSchema(schemaName.c_str(), optr);
	if (errcode != 0) 
	{
		debug_printf(*ec, "[QE] TNSCHEMANODE - error in addSchema");
		return errcode;
	}

	OuterSchema schema;
	OuterSchema *s = &schema;
	errcode = OuterSchema::fromLogicalID(schemaLid, tr, s);
	if (errcode) return errcode;
	STATE state = OuterSchemaValidator::validate(s, tr, true);
	debug_printf(*ec, "[QE] outer schema validated");
	addSchema(*s);
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

void OuterSchemas::addSchema(OuterSchema s)
{
	string name = s.getName();
	m_outerSchemas[name] = s;
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
		OuterSchema *schP = &sch;
		STATE s = OuterSchemaValidator::validate(schP, tr);
		if (OuterSchemaValidator::isError(s)) 
			sch.setValidity(OUTERSCHEMA_INVALID_OTHER);
		m_outerSchemas[name] = sch;
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

int OuterSchemas::exportSchema(string schemaName, TManager::Transaction *tr) const
{
	if (!hasSchemaName(schemaName))
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] no schema by name %s\n", schemaName.c_str());
		return -1;  //TODO - no schema by this name
	}
	OuterSchema s = getSchema(schemaName);
	/*
	OuterSchema *sp = &s;
	if (!OuterSchemaValidator::validate(sp, tr))
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] schema %s is invalid and cannot be exported\n", schemaName.c_str());
		return -1; //TODO - invalid schema can't be exported
	}
	*/
	string exportedContent = s.toString(tr);
	string fileName = getSchemaFile(schemaName);
	ofstream file;
	file.open(fileName.c_str(), ios::out | ios::trunc);
	if (!file.is_open())
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] could not open file %s\n", fileName.c_str());
		return -1; //TODO - couldn't open file
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
		debug_printf(*ec, "[OuterSchemas::importSchema] schema by name %s already exists\n", schemaName.c_str());
		return -1;  //TODO - schema already exists
	}
	string schemaString, line;
	string fileName = getSchemaFile(schemaName);
	ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open())
	{
		debug_printf(*ec, "[OuterSchemas::exportSchema] could not open file %s\n", fileName.c_str());
		return -1; //TODO - couldn't open file
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
		if (t != BIND_INTERFACE) {s->setValidity(OUTERSCHEMA_INVALID_OTHER); return ERROR_NOT_INTERFACE;} //Only interfaces are allowed in schema
		if (!sp->getAssociatedObjectName().empty())
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



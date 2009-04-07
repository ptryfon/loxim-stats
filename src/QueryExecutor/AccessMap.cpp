#include "AccessMap.h"
#include "QueryBuilder.h"
#include "QueryResult.h"
#include "OuterSchema.h"
#include "InterfaceMaps.h"
#include "QueryExecutor.h"
#include "Errors/ErrorConsole.h"

using namespace Schemas;
namespace QExecutor
{
	
	string AccessMap::getCrudString(int crud)
	{
		string res = "";
		if (crud & CREATE) res += "create";
		if (crud & READ) res += " read";
		if (crud & UPDATE) res += " update";
		if (crud & DELETE) res += " delete";
		return res;
	}
	
	int AccessMap::getCrudFromName(string name)
	{
		if (name == "Create") return CREATE;
		if (name == "Read") return READ;
		if (name == "Update") return UPDATE;
		if (name == "Delete") return DELETE;
		return NO_ACCESS;
	}
	
	bool canCreate(int crud)	{ return (crud & CREATE); }
	bool canRead(int crud)		{ return (crud & READ); }
	bool canUpdate(int crud)	{ return (crud & UPDATE); }
	bool canDelete(int crud)	{ return (crud & DELETE); }
	
	AccessMap::AccessMap()
	{
		ec = &ErrorConsole::get_instance(EC_QUERY_EXECUTOR); 
		m_isDba = false;
		m_procedureAccessControl = false;
		m_disabled = false;
		m_procLevel = -1;
		m_disabledLevel = -1;
	}
	
	bool AccessMap::isSchemaValid() const
	{
		if (m_isDba || m_userSchemaName.empty())
			return true;
		return OuterSchemas::Instance().isValid(m_userSchemaName);
	}
	
	int AccessMap::getAccess(string name) const
	{
		if (m_disabled) return FULL_ACCESS;
		TNameToExtendedAccess::const_iterator it = m_accessMap.find(name);
		if (it == m_accessMap.end())
		{
			if (name == getUserTableName())
				return m_isDba ? FULL_ACCESS : READ;
			it = m_baseMap.find(name);
			if (it == m_baseMap.end())
				return m_isDba ? FULL_ACCESS : NO_ACCESS;
		}
		int access = it->second.first;
		if (access == PROCEDURE_ONLY)
		{
			if (m_procedureAccessControl)
			{
				debug_printf(*ec, "Procedure access control is on");
				return FULL_ACCESS;
			}
			else
				return NO_ACCESS; //even for m_dba - becouse it is through interface
		}
		return m_isDba ? FULL_ACCESS : access;
	}
	
	bool AccessMap::canGiveAccess(string objectName) const
	{
		if (m_isDba) return true;
		TNameToExtendedAccess::const_iterator it = m_accessMap.find(objectName);
		if (it == m_accessMap.end())
		{
			it = m_baseMap.find(objectName);
			if (it == m_baseMap.end())
				return false;
		}
		return it->second.second;	
	}
	
	bool AccessMap::canRevokeAccess() const
	{
		return m_isDba;	
	}
	
	bool AccessMap::hasAccess(int access, string objectName) const
	{
		int crud = getAccess(objectName);
		debug_printf(*ec, "user is %s, asking for %d access to %s, found rights = %d, cross = %d", m_isDba  ? "dba" : "not dba", access, objectName.c_str(), crud, crud & access);
		return (access == (crud & access));		
	}
	
	bool AccessMap::canCreateUser() const
	{		
		return hasAccess(CREATE, getUserTableName());
	}
	
	bool AccessMap::canRemoveUser() const
	{
		return hasAccess(DELETE, getUserTableName());
	}
	
	int AccessMap::mergeCruds(int crud1, int crud2) const
	{
		if ((crud1 == PROCEDURE_ONLY) || (crud2 == PROCEDURE_ONLY)) return PROCEDURE_ONLY;
		int crud = 0;
		if (canCreate(crud1) || canCreate(crud2)) crud += CREATE;		
		if (canRead(crud1) || canRead(crud2)) crud += READ;
		if (canUpdate(crud1) || canUpdate(crud2)) crud += UPDATE;
		if (canDelete(crud1) || canDelete(crud2)) crud += DELETE;
		return crud;
	}
	
	void AccessMap::addAccess(string name, int crud, bool canGive, bool base)
	{
		debug_printf(*ec, "[AccessMap::addAccess]: adding access type %d for user %s to name %s", crud, m_user.c_str(), name.c_str());
		if (base)
		{
			TNameToExtendedAccess::iterator it = m_baseMap.find(name);
			if (it == m_baseMap.end())
				m_baseMap[name] = TAccess(crud, canGive);
			else 
			{
				int oldCrud = it->second.first;
				m_baseMap[name] = TAccess(mergeCruds(crud, oldCrud), canGive);
			}
			return;
		}
		TNameToExtendedAccess::iterator it = m_accessMap.find(name);
		if (it == m_accessMap.end())
			m_accessMap[name] = TAccess(crud, canGive);
		else 
		{
			int oldCrud = it->second.first;
			m_accessMap[name] = TAccess(mergeCruds(crud, oldCrud), canGive);
		}
	}
	
	void AccessMap::addAccessList(set<string> names, int crud, bool canGive)
	{
		set<string>::iterator it;
		for (it = names.begin(); it != names.end(); ++it)
		{
			addAccess(*it, crud, canGive, false);
		}
	}
	
	void AccessMap::propagateAccess(string father, map<string, bool> children)
	{   //false value in map indicates that this field is not visible through interface and accessible only by procedures
		//debug_printf(*ec, "[AccessMap::propagateAccess]: adding %d children for %s", children.size(), father.c_str());
		int crud = getAccess(father);
		if (crud == NO_ACCESS) return;  //Shouldn't happen
		
		set<string> accessibleChildren;
		set<string> procedureOnlyChildren;
		for (map<string, bool>::iterator it = children.begin(); it != children.end(); ++it)
		{
			string name = it->first;
			bool procedureOnly = !(it->second);
			if (procedureOnly)
				procedureOnlyChildren.insert(name);
			else
				accessibleChildren.insert(name);
		}
		addAccessList(accessibleChildren, crud);	
		addAccessList(procedureOnlyChildren, PROCEDURE_ONLY);
	}
	
	void AccessMap::removeSection(int secNo)
	{
		debug_printf(*ec, "[AccessMap::removeSection]: removing section no. %d", secNo);
		TIntToStringSet::iterator it = m_sectionToNamesMap.find(secNo);
		if (it != m_sectionToNamesMap.end())
		{
			set<string> names = it->second;
			set<string>::iterator si;
			for (si = names.begin(); si != names.end(); ++si)
			{
				string name = *si;
				m_accessMap.erase(name);
			}
			m_sectionToNamesMap.erase(secNo);
		}
	}
	
	void AccessMap::removeAllSections()
	{	
		set<int> keys;
		for (TIntToStringSet::iterator it = m_sectionToNamesMap.begin(); it != m_sectionToNamesMap.end(); ++it)
			keys.insert(it->first);	
		for (set<int>::iterator it = keys.begin(); it != keys.end(); ++it)
			removeSection(*it);
	}
	
	void AccessMap::addSectionInfo(int secNo, set<string> names)
	{
		m_sectionToNamesMap[secNo] = names;
	}
	
	void AccessMap::resetForSchema(string schemaName)
	{   //asserting: schema exists and is valid
		reset();
		m_baseMap.clear();
		
		OuterSchemas &os = OuterSchemas::Instance();
		OuterSchema s = os.getSchema(schemaName);
		TNameToAccess ap = s.getAccessPoints();
		m_baseMap[schemaName] = TAccess(READ, false);
		for (TNameToAccess::iterator it = ap.begin(); it != ap.end(); ++it)
		{
			string name = it->first;
			int crud = it->second;
			InterfaceMaps &im = InterfaceMaps::Instance();
			if (im.hasInterface(name))
			{
				bool found;
				string objectName = im.getObjectNameForInterface(name, found);
				if (found)
				{
					if (!objectName.empty())
						addAccess(objectName, crud, false, true);
					addAccess(name, READ, false, true);
				}
			}
			else if (im.hasObjectName(name))
			{
				bool found;
				string interfaceName = im.getInterfaceNameForObject(name, found);
				if (found)
				{
					addAccess(name, crud, false, true);
					addAccess(interfaceName, READ, false, true);
				}			
			}
		}
	}
	
	void AccessMap::reset()
	{
		m_accessMap.clear();
		m_sectionToNamesMap.clear();
	}
	
	int AccessMap::resetForUser(string username, QueryExecutor *qe)
	{
		debug_printf(*ec, "[AccessMap::resetForUser] starts!");	
		reset();
		m_user = username;
		m_isDba = username == "root" ? true :false;
		if (m_isDba) return 0;
		
		//TODO - do bindNames zabrac to i stringi z QueryBuilder'a
		const string privNameBind = "priv_name";
		const string objectNameBind = "object_name";
		const string grantOptionBind = "grant_option";
		
		debug_printf(*ec, "[AccessMap::resetForUser] executing..");	
		string query = QueryBuilder::getHandle()->query_for_user_priviliges(username);
		QueryResult *res;
		qe->execute_locally(query, &res);
		debug_printf(*ec, "[AccessMap::resetForUser] executed..");	
		
		if (res->type() != QueryResult::QSTRUCT)
		{
			debug_printf(*ec, "[AccessMap::resetForUser] error: QSTRUCT expected!");
			return (ErrQExecutor | EOtherResExp);			
		}
		QueryStructResult *currStructResult = (QueryStructResult *)res;
		string privName, objectName;
		bool canGive = false;
		while (!(currStructResult->isEmpty()))
		{
			debug_printf(*ec, "[AccessMap::resetForUser] loops..");
			QueryResult *innerResult;
			currStructResult->getResult(innerResult);
			if (innerResult->type() != QueryResult::QBINDER)
			{
				debug_printf(*ec, "[AccessMap::resetForUser] error: QBINDER expected!");
				return (ErrQExecutor | EOtherResExp);		
			}
			QueryBinderResult *innerBinderResult = (QueryBinderResult *)innerResult;
			QueryResult *item = innerBinderResult->getItem();
			string name = innerBinderResult->getName();
			if (name == privNameBind)
			{
				QueryStringResult *s = (QueryStringResult *)item;
				privName = s->getValue();
			}
			else if (name == objectNameBind)
			{
				QueryStringResult *s = (QueryStringResult *)item;
				objectName = s->getValue();
			}
			else if (name == grantOptionBind)
			{
				QueryIntResult *s = (QueryIntResult *)item;
				canGive = (s->getValue() == 1);				
			}
		}
		int crud = getCrudFromName(privName);
		debug_printf(*ec, "[AccessMap::resetForUser] Adding access %s for user %s to schema %s", privName.c_str(), username.c_str(), objectName.c_str()); 
		if (canGive) debug_printf(*ec, "[AccessMap::resetForUser] with right to give access"); 	
		addAccess(objectName, crud, canGive, true);
		if (!OuterSchemas::Instance().hasSchemaName(objectName))
		{
			debug_printf(*ec, "[AccessMap::resetForUser] no such schema (user rights invalid)!");		
			return (ErrQExecutor | ENoSchemaFound);		
		}
		if (!OuterSchemas::Instance().isValid(objectName))
		{
			debug_printf(*ec, "[AccessMap::resetForUser] user is not allowed to access db through this schema (schema invalid)!");		
			return (ErrQExecutor | EUserHasInvalidSchema);
		}
		resetForSchema(objectName);
		
		debug_printf(*ec, "[AccessMap::resetForUser] ends!");
		return 0;
	}
	
	set<string> AccessMap::namesFromBinders(QueryResult *bindersBagResult)
	{
		set<string> out;
		QueryBagResult *r = (QueryBagResult *)bindersBagResult;
		if (!r) return out;
		unsigned bagSize = r->size();
	
		for(unsigned int i = 0; i < bagSize; i++) 
		{
			QueryResult* br;
			r->at(i, br);
			if(br->type() != QueryResult::QBINDER) 
				return out;
			else
			{
				string name = (((QueryBinderResult *) br)->getName());
				out.insert(name);
			}
		}
		return out;
	}

	void AccessMap::addSectionInfo(int secNo, QueryResult *r)
	{
		if (m_isDba) return;
		return addSectionInfo(secNo, namesFromBinders(r));
	}
	
	void AccessMap::setAccessControl(bool procedureOnly, bool enable, int level)
	{
		if (procedureOnly)
		{
			debug_printf(*ec, "setAccessControl: procedureOnly %s, %d", enable ? "enabled" : "disabled", level); 
			if ((m_procLevel == -1) || (level == m_procLevel))
			{
				m_procedureAccessControl = enable;
				if (!enable)
					m_procLevel = -1;
				else
					m_procLevel = level;
			}
		}
		else
		{
			debug_printf(*ec, "setAccessControl: fullAccess %s, %d", enable ? "enabled" : "disabled", level); 
			if ((m_disabledLevel == -1) || (level == m_disabledLevel))
			{
				m_disabled = enable;
				if (!enable)
					m_disabledLevel = -1;
				else
					m_disabledLevel = level;
			}
		}
	}
	
	void AccessMap::enableProcedureOnly(int sec) {return setAccessControl(true, true, sec);}
	void AccessMap::disableProcedureOnly(int sec) {return setAccessControl(true, false, sec);}
	void AccessMap::disableAccessControl(int sec) {return setAccessControl(false, true, sec);}
	void AccessMap::enableAccessControl(int sec) {return setAccessControl(false, false, sec);}
	
}

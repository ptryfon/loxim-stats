#ifndef _ACCESSMAP_H_
#define _ACCESSMAP_H_

#include <string>
#include <set>
#include <map>

/*
	Simple user access rights management class. 
	It should be replaced with EnvironmentStack extension (reacting to sections
	popping and pushing) when someone starts rewriting access rights in Loxim.
*/

namespace Errors {class ErrorConsole;}
namespace QExecutor {class QueryExecutor; class QueryResult;}
using namespace Errors;
using namespace std;

namespace QExecutor
{
	typedef pair<int, bool> TAccess;
	typedef map<string, TAccess> TNameToExtendedAccess;
	typedef map<int, set<string> > TIntToStringSet;
	
	enum CRUD
	{
		NO_ACCESS   =   0,
		CREATE		=   1,
		READ		=   2,
		UPDATE		=   4,
		DELETE		=   8,
		FULL_ACCESS	=   CREATE + READ + UPDATE + DELETE
	};

	static string getCrudString(int crud)
	{
		string res = "";
		if (crud & CREATE) res += "create";
		if (crud & READ) res += " read";
		if (crud & UPDATE) res += " update";
		if (crud & DELETE) res += " delete";
		return res;
	}
	
	static int getCrudFromName(string name)
	{
		if (name == "Create") return CREATE;
		if (name == "Read") return READ;
		if (name == "Update") return UPDATE;
		if (name == "Delete") return DELETE;
		return NO_ACCESS;
	}
	
	bool canCreate(int crud);
	bool canRead(int crud);
	bool canUpdate(int crud);
	bool canDelete(int crud);
	
	class AccessMap
	{
		private:
			TNameToExtendedAccess m_baseMap;
			TNameToExtendedAccess m_accessMap;
			TIntToStringSet m_sectionToNamesMap;
			string m_user;
			string m_userSchemaName;
			bool m_isDba;
			ErrorConsole *ec;
			
			int mergeCruds(int crud1, int crud2) const;	
			
			void resetForSchema(string schemaName);
			void addSectionInfo(int secNo, set<string> names);
			
			static string getUserTableName() {return "xuser";}
			static set<string> namesFromBinders(QueryResult *bindersBagResult);
	
		public:
			AccessMap();
			
			bool isSchemaValid() const;

			bool hasAccess(int access, string objectName) const;
			bool isDba() const {return m_isDba;}
			
			bool canCreateUser() const;
			bool canRemoveUser() const;
			bool canGiveAccess(string objectName) const;
			bool canRevokeAccess() const;
			
			void addAccessList(set<string> names, int crud, bool grant = false);
			void addSectionInfo(int secNo, QueryResult *r);
			void propagateAccess(string father, set<string> children);			
			void removeSection(int secNo);
			void removeAllSections();

			void addAccess(string name, int crud, bool grant = false, bool base = false);
			int getAccess(string name) const;
			
			void reset();
			int resetForUser(string username, QueryExecutor *qe);
			
	};
}

#endif //_ACCESSMAP_H_

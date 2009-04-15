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
namespace QExecutor {class QueryExecutor; class QueryResult; class QueryVirtualResult;}
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
		FULL_ACCESS	=   CREATE + READ + UPDATE + DELETE,
		PROCEDURE_ONLY = 16
	};

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
			
			bool m_procedureAccessControl;
			int m_procLevel;
			bool m_disabled;
			int m_disabledLevel;
			void setAccessControl(bool procedureOnly, bool enable, int level);		
			
			bool m_isDba;
			ErrorConsole *ec;
			
			int mergeCruds(int crud1, int crud2) const;	
			
			void resetForSchema(string schemaName);
			void addSectionInfo(int secNo, set<string> names);
			void adjustEnvironment(QueryVirtualResult *r, bool add);

			void removeAccess(set<string> children);
			void removeAccess(string name, bool base = false);

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
			void propagateAccess(string father, map<string, bool> children);		
			void removeSection(int secNo);
			void removeAllSections();
			
			void addViewEnvironment(QueryVirtualResult *r);
			void removeViewEnvironment(QueryVirtualResult *r);

			void addAccess(string name, int crud, bool grant = false, bool base = false);
			int getAccess(string name) const;

			void enableProcedureOnly(int sec);
			void disableProcedureOnly(int sec);
			void disableAccessControl(int sec);
			void enableAccessControl(int sec);
			
			void reset();
			int resetForUser(string username, QueryExecutor *qe);

			static string getCrudString(int crud);	
			static int getCrudFromName(string name);		
	};
}

#endif //_ACCESSMAP_H_

#ifndef _OUTERSCHEMA_H_
#define _OUTERSCHEMA_H_
#include <map>
#include "BindMap.h"
#include "BindNames.h"

namespace Errors
{
	class ErrorConsole;
}

namespace TManager
{
	class Transaction;
}

namespace Store
{
	class LogicalID;
	class ObjectPointer;
}

using namespace Errors;
using namespace std;

namespace Schemas
{

	enum EntityType
	{
		INTERFACE,
		CLASS,
		VIEW,
		OBJECT	
	};

	enum STATE 
	{
		VALID = 0,
		INVALID_OTHER,
		INVALID_NO_BIND,
		INVALID_BAD_BIND,
		ERROR_NAME_NOT_UNIQUE,
		ERROR_TM,
		ERROR_NO_NAME,
		ERROR_NOT_INTERFACE
	};
	
	typedef map<string, int> TNameToAccess;	

	class OuterSchema
	{
		private:
			TNameToAccess m_namesInSchema;
			string m_name;
			
			void addName(string name, int crudFlags);
			int getCrudForName(string name) const;
			
		public:
			OuterSchema(string name="no_name") {m_name = name; }
			OuterSchema(string name, TNameToAccess aps) {m_name = name; setAccessPoints(aps); }
			void addName(string name, EntityType type, bool canCreate = true, bool canRead = true, bool canUpdate = true, bool canDelete = true);
			void setName(string n) {m_name = n;}
			void setAccessPoints(TNameToAccess aps) {m_namesInSchema = aps;}

			void removeName(string name);
			string getName() const {return m_name;}
			TNameToAccess getAccessPoints() const {return m_namesInSchema;}

			bool nameVisible(string name) const;
			bool canCreate(string name) const;
			bool canUpdate(string name) const;
			bool canRead(string name) const;
			bool canDelete(string name) const;
			
			void print(ErrorConsole *ec) const;
			string toString(TManager::Transaction *tr) const;

			static int fromLogicalID(Store::LogicalID *lid, TManager::Transaction *tr, OuterSchema *&s);

	};

	typedef map<string, set<string> > TNameInSchemas;
	typedef map<string, OuterSchema> TOuterSchemas;
	typedef map<string, STATE> TValidityMap;
	
	class OuterSchemas
	{
		private:
		    OuterSchemas();
		    OuterSchemas(const OuterSchemas &oS);
		    OuterSchemas& operator=(const OuterSchemas &oS);
		    
		    int loadOuterSchemas(TManager::Transaction *tr, TLidsVector *lvec);
			static string getSchemaFile(string schemaName) {return schemaName + ".sch";}
		
			void addSchema(OuterSchema s, STATE validity);
			void removeSchema(string name);

			STATE getValidity(string schemaName) const;
		
		    TOuterSchemas m_outerSchemas;
			TNameInSchemas m_namesInSchemas;
			TValidityMap m_schemasValidity;
		    ErrorConsole *ec;
		    
		public:
			static OuterSchemas& Instance() {static OuterSchemas outerSchemasInstance; return outerSchemasInstance;} 		
			int init();
			void clearSchemas() {m_outerSchemas.clear();}
			void deinit();
			
			int addSchema(LogicalID *lid, TManager::Transaction *tr, int createType);
			int removeSchema(string name, TManager::Transaction *tr, ObjectPointer *p);
			int removeSchema(string name, TManager::Transaction *tr);
			
			bool isValid(string schemaName) const;

			bool hasSchemaName(const string &s) const {return (m_outerSchemas.find(s) != m_outerSchemas.end());}
			OuterSchema getSchema(string name) const;
			set<string> getAllSchemasUsingName(string s) const;
			void revalidateAllSchemasUsingName(string s, TManager::Transaction *tr);
			
			int exportSchema(string schemaName, TManager::Transaction *tr) const;
			int importSchema(string schemaName, string &out) const;

			string toString(TManager::Transaction *tr) const;
			void debugPrint(TManager::Transaction *tr) const;
			
	};
		
	class OuterSchemaValidator
	{
		public:
			static STATE validate(const OuterSchema &s, TManager::Transaction *tr, bool beforeAdding = false);
			static bool isError(STATE s) {return (s >= ERROR_NAME_NOT_UNIQUE);}
			static bool isInvalid(STATE s) {return (s >= INVALID_OTHER);}
		private:
			static STATE validateInterfaceBind(string interfaceName, TManager::Transaction *tr);
	};
}

#endif //_OUTERSCHEMA_H_

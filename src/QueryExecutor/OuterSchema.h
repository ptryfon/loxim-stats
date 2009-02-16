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
}

using namespace Errors;
using namespace std;

namespace Schemas
{
	enum CRUD
	{
		CREATE  =   1,
		READ	=   2,
		UPDATE  =   4,
		DELETE  =   8
	};

	static string getCrudString(int crud)
	{
		string res = "";
		if (crud & CREATE) res += "CREATE";
		if (crud & READ) res += "READ";
		if (crud & UPDATE) res += "UPDATE";
		if (crud & DELETE) res += "DELETE";
		return res;
	}
	
	enum EntityType
	{
		INTERFACE,
		CLASS,
		VIEW,
		OBJECT	
	};

	enum SchemaValidity
	{
		OUTERSCHEMA_VALID,
		OUTERSCHEMA_INVALID_NO_BIND,
		OUTERSCHEMA_INVALID_BAD_BIND,
		OUTERSCHEMA_INVALID_OTHER
	};
	
	typedef map<string, int> TNameToAccess;	

	class OuterSchema
	{
		private:
			TNameToAccess m_namesInSchema;
			string m_name;
			SchemaValidity m_validity;
			
			void addName(string name, int crudFlags);
			int getCrudForName(string name) const;
			
		public:
			OuterSchema(string name="no_name") {m_name = name; m_validity = OUTERSCHEMA_INVALID_OTHER;}
			OuterSchema(string name, TNameToAccess aps) {m_name = name; setAccessPoints(aps); m_validity = OUTERSCHEMA_INVALID_OTHER;}
			void addName(string name, EntityType type, bool canCreate = true, bool canRead = true, bool canUpdate = true, bool canDelete = true);
			void setName(string n) {m_name = n;}
			void setAccessPoints(TNameToAccess aps) {m_namesInSchema = aps;}
			void setValidity(SchemaValidity v) {m_validity = v;}

			void removeName(string name);
			string getName() const {return m_name;}
			SchemaValidity getValidity() const {return m_validity;}
			TNameToAccess getAccessPoints() const {return m_namesInSchema;}

			bool nameVisible(string name) const;
			bool canCreate(string name) const;
			bool canUpdate(string name) const;
			bool canRead(string name) const;
			bool canDelete(string name) const;
			
			void print(ErrorConsole *ec) const;
			string toString(TManager::Transaction *tr) const;

			static const int defaultCrud = READ; //read-only access
			static const int noAccess = 0;

			static int fromLogicalID(Store::LogicalID *lid, TManager::Transaction *tr, OuterSchema *&s);

	};
	
	typedef map<string, set<string> > TNameInSchemas;
	typedef map<string, OuterSchema> TOuterSchemas;
	
	class OuterSchemas
	{
		private:
		    OuterSchemas();
		    OuterSchemas(const OuterSchemas &oS);
		    OuterSchemas& operator=(const OuterSchemas &oS);
		    
		    int loadOuterSchemas(TManager::Transaction *tr, TLidsVector *lvec);
			static string getSchemaFile(string schemaName) {return schemaName + ".sch";}
		
		    TOuterSchemas m_outerSchemas;
			TNameInSchemas m_namesInSchemas;
		    ErrorConsole *ec;
		    
		public:
			static OuterSchemas& Instance() {static OuterSchemas outerSchemasInstance; return outerSchemasInstance;} 		
			int init();
			void clearSchemas() {m_outerSchemas.clear();}
			void deinit();
			
			void addSchema(OuterSchema s);
			void removeSchema(string name);
			bool hasSchemaName(const string &s) const {return (m_outerSchemas.find(s) != m_outerSchemas.end());}
			OuterSchema getSchema(string name) const;
			set<string> getAllSchemasUsingName(string s) const;
			void revalidateAllSchemasUsingName(string s, TManager::Transaction *tr);
			
			int exportSchema(string schemaName, TManager::Transaction *tr) const;
			int importSchema(string schemaName, TManager::Transaction *tr) const;

			string toString(TManager::Transaction *tr) const;
			void debugPrint(TManager::Transaction *tr) const;
			
	};
	
	enum STATE {VALID, INVALID, ERROR_NAME_NOT_UNIQUE, ERROR_TM, ERROR_NO_NAME, ERROR_NOT_INTERFACE};		
	
	class OuterSchemaValidator
	{
		public:
			static STATE validate(OuterSchema *&s, TManager::Transaction *tr, bool isNew = false);
			static bool isError(STATE s) {return (s >= ERROR_NAME_NOT_UNIQUE);} 
		private:
			static SchemaValidity validateInterfaceBind(string interfaceName, TManager::Transaction *tr);
	};
}

#endif //_OUTERSCHEMA_H_

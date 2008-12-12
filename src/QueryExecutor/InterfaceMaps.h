#ifndef INTERFACE_MAPS_H
#define INTERFACE_MAPS_H

// Some methods from this classes assume that ClassGraph was initialized earlier

#include <map>
#include <set>
#include <vector>
#include <string>
#include "InterfaceMatcher.h"
#include "BindMap.h"

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

using namespace std;
using namespace Errors;
using namespace TManager;
using namespace Store;

typedef set<string> TStringSet;

namespace Schemas
{
	class Schema;

	class HierarchyInfo
	{
		private:
			TStringSet m_parents;
			TStringSet m_children;
		
		public:
			HierarchyInfo() {}
			bool hasParent(const string &p) const {return m_parents.find(p) != m_parents.end();}
			bool hasChild(const string &c) const {return m_children.find(c) != m_children.end();}
			void addParent(string p) {m_parents.insert(p);}
			void addChild(string c) {m_children.insert(c);}
			void removeParent(string p) {m_parents.erase(p);}
			void removeChild(string c) {m_children.erase(c);}
			TStringSet getParents() const {return m_parents;} 
			TStringSet getChildren() const {return m_children;}
	};

	typedef map<string, TFields> TNameToFields;
	typedef map<string, TMethods> TNameToMethods;

	class InterfaceMembersCache
	{   //TODO - const pointers?, cache methods by name and args
		public:
			InterfaceMembersCache() {}
			void addResult(string interfaceName, TFields fields) {m_interfaceFields[interfaceName] = fields;}
			void addResult(string interfaceName, TMethods methods) {m_interfaceMethods[interfaceName] = methods;}
			
			void clear() {m_interfaceFields.clear(); m_interfaceMethods.clear();}

			bool hasFieldsFor(string interfaceName) const {return (m_interfaceFields.find(interfaceName) != m_interfaceFields.end());}
			bool hasMethodsFor(string interfaceName) const {return (m_interfaceMethods.find(interfaceName) != m_interfaceMethods.end());}
			
			TFields getFieldsFor(string interfaceName) {return m_interfaceFields[interfaceName];}
			TMethods getMethodsFor(string interfaceName) {return m_interfaceMethods[interfaceName];}

		private:
			TNameToFields m_interfaceFields;
			TNameToMethods m_interfaceMethods;
	};

	typedef map<string, Schema> TInterfaceToSchemas;
	typedef map<string, HierarchyInfo> TInterfaceToHierarchy;
	typedef map<string, TStringSet> TNameInHierarchy;

	class InterfaceMaps
	{
		public:
			static InterfaceMaps& Instance() {static InterfaceMaps interfaceMapsInstance; return interfaceMapsInstance;} 
			
			int init();
			void deinit();
			void clearMaps();

			bool checkIfCanBeInserted(Schema interfaceSchema) {return insertNewInterfaceAndPropagateHierarchy(interfaceSchema, true, true);} 
			bool insertNewInterfaceAndPropagateHierarchy(Schema interfaceSchema, bool checkValidity = true, bool tryOnly = false);
			int removeInterface(string interfaceName, bool checkValidity = true);
			
			int addBind(string objectName, TManager::Transaction *tr);
			void addBind(string interfaceName, string impName, string impObjName, int type);
			void removeBind(string objectName) {m_bindMap.removeBind(m_objNameToName[objectName]);}
			bool isInterfaceBound(string n) const {return m_bindMap.hasBind(n);}
			bool isObjectNameBound(string o) const;
			void getInterfaceBindForObjName(string oName, string& iName, string& cName, string &invName, int &type, bool &found, bool final = false) const;
			ImplementationInfo getImplementationForInterface(string name, bool &found, bool final = false) const;
			void getClassBindForInterface(string interfaceName, LogicalID*& classGraphLid) const;
			void updateAsImplementationIsRemoved(string implementationName) {m_bindMap.removeEntriesForImplementation(implementationName);}
			//BindMap getBindMap() const {return m_bindMap;}  
			
			string getObjectNameForInterface(string i, bool &found) const;
			string getInterfaceNameForObject(string i, bool &found) const;
			bool hasInterface(string n) const {return (m_nameToSchema.find(n) != m_nameToSchema.end());}
			bool hasObjectName(string n) const {return (m_objNameToName.find(n) != m_objNameToName.end());}
			
			TStringSet getAllFieldNamesAndCheckBind(string interfaceKey, bool &filterOut) const;
			TFields getFieldsIncludingDerived(string interfaceName, bool cache = true) const;
			TMethods getMethodsIncludingDerived(string interfaceName, bool cache = true) const;
			Schema getSchemaForInterface(string interfaceName) const;
			bool interfaceHasMethod(string interfaceName, string methodName, int args) const;

			void printAll() const;

		private:
		    InterfaceMaps();
		    InterfaceMaps(const InterfaceMaps &iM);
		    InterfaceMaps& operator=(const InterfaceMaps &iM);
		    
		    int loadSchemas(TManager::Transaction *tr, TLidsVector *lvec);
			void clearMap();
			bool checkHierarchyValidity() const;
			TStringSet getAllExt(string intName) const;

			TDict m_objNameToName;
			TInterfaceToSchemas m_nameToSchema;
			TInterfaceToHierarchy m_nameToHierarchy;
			TNameInHierarchy m_nameToInterfacesExtending;

			mutable InterfaceMembersCache m_cache;

			BindMap m_bindMap;

			ErrorConsole *ec;
	};


	

}


#endif //INTERFACE_MAPS_H


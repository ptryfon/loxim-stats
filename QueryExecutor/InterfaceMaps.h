#ifndef INTERFACE_MAPS_H
#define INTERFACE_MAPS_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include "InterfaceMatcher.h"

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
	
	typedef vector<LogicalID*> TLidsVector;

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

	typedef map<string, string> TDict;
	typedef map<string, Schema> TInterfaceToSchemas;
	typedef map<string, HierarchyInfo> TInterfaceToHierarchy;
	typedef map<string, TStringSet> TNameInHierarchy;

	//For future extensions
	class ImplementationInfo
	{
		private:
			string m_name;
			string m_invName;
		
		public:
			ImplementationInfo() {}
			void setName(string n) {m_name = n;}
			void setInvName(string n) {m_invName = n;}
			string getName() const {return m_name;}
			string getInvName() const {return m_invName;}
	};
	
	typedef map<string, ImplementationInfo> TIntToImpInfo;

	class BindMap
	{
		private:
			TDict m_objNameToInt;
			TIntToImpInfo m_intToImp;

		public:
			BindMap() {clear();}
			void addBind(string interface, string obj, string impl, string inv);
			void removeBind(const string& obj);
			bool hasBind(const string& interface) const {return (m_intToImp.find(interface) != m_intToImp.end());}
			bool hasObjectBind(const string& objectName) const {return (m_objNameToInt.find(objectName) != m_objNameToInt.end());}			
			void clear();

			string getIntForObj(const string& oName) const;
			string getInvForInt(const string& intName) const;
			string getClassForInt(const string& intName) const;

			void print(ErrorConsole *ec) const;
	};

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
			
			//TODO - per outer scheme!
			int addBind(string objectName, TManager::Transaction *tr);
			void addBind(string interfaceName, string objectName, string className, string invName);
			void removeBind(string objectName) {m_bindMap.removeBind(objectName);}
			bool isInterfaceBound(string n) const {return m_bindMap.hasBind(n);}
			bool isObjectNameBound(string o) const {return m_bindMap.hasObjectBind(o);}
			void getInterfaceBindForObjName(string oName, string& iName, string& cName, string &invName) const;
			void getClassBindForInterface(string interfaceName, LogicalID*& classGraphLid) const;
			BindMap getBindMap() const {return m_bindMap;}  
			
			string getObjectNameForInterface(string i) {return m_nameToSchema[i].getAssociatedObjectName();}
			bool hasInterface(string n) const {return (m_nameToSchema.find(n) != m_nameToSchema.end());}
			
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

			TInterfaceToSchemas m_nameToSchema;
			TInterfaceToHierarchy m_nameToHierarchy;
			TNameInHierarchy m_nameToInterfacesExtending;

			mutable InterfaceMembersCache m_cache;

			BindMap m_bindMap;  //TODO - per outer scheme!

			ErrorConsole *ec;
	};


	

}


#endif //INTERFACE_MAPS_H


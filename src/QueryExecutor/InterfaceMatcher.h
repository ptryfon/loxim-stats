#ifndef _INTERFACE_MATCHER_H_
#define _INTERFACE_MATCHER_H_

// Some methods from this classes assume that ClassGraph was initialized earlier

#include <vector>
#include <set>
#include <string>

namespace Errors
{
    class ErrorConsole;
}

namespace Store
{
	class LogicalID;
}

namespace QParser
{
	class InterfaceNode;
}

namespace QExecutor
{
	class QueryBagResult;
	class QueryResult;
	class QueryExecutor;
	class ClassGraphVertex;
}

namespace TManager
{
	class Transaction;
}

using namespace std;

namespace Schemas
{    
    class Member 
    {
	public:
	    string getName() const {return m_name;}
	protected:
	    Member() {};
	    string m_name;
    };

    class Field : public Member
    {
	public:
	    bool Matches(Field other) const {return m_name == other.getName();}
	    Field(string n) {m_name=n;}
    };
    typedef vector<Field *> TFields;
    
    class Method : public Member
    {
	public:
	    bool Matches(Method other) const;
	    Method() {};
	    Method(string n) {m_name = n; m_paramsSorted = true;}
	    void setName(string n) {m_name = n;} 
	    void addParam(Field *f) {m_params.push_back(f); m_paramsSorted = false;}
	    void sortParams();
	    bool paramsSorted() const {return m_paramsSorted;}  
	    int getParamsCount() const {return m_params.size();}
	    TFields getParams() const {return m_params;}
	protected:
	    bool m_paramsSorted;
	
	    TFields m_params;
    };
    typedef vector<Method *> TMethods;
    typedef set<string> TSupers;

    class Schema 
    {
	public:
	    Schema(int schemaType = 0);
	    ~Schema();
	    TFields getFields() const {return m_fields;}
	    TMethods getMethods() const {return m_methods;}   
	    TSupers getSupers() const {return m_supers;}
	    string getName() const {return m_name;}
	    string getAssociatedObjectName() const {return m_associatedObjectName;}

	    void setSchemaType(int t) {m_schemaType = t;}
	    void setAssociatedObjectName(string name) {m_associatedObjectName = name;}
	    void setName(string n) {m_name = n;}    
		void setMethods(TMethods m) {m_methods = m;}
		void setFields(TFields f) {m_fields = f;}
		void setSupers(TSupers s) {m_supers = s;}

		void addField(Field *f) {m_fields.push_back(f);}
	    void addMethod(Method *m) {m_methods.push_back(m);}
	    void addSuper(string s) {m_supers.insert(s);}

		void printAll(Errors::ErrorConsole *ec) const; 
		string toSchemaString() const;
	    void sortVectors();
	    void join(Schema *s, bool sort = true);

		static int interfaceMatchesImplementation(string interfaceName, string implementationName, TManager::Transaction *tr, int type, bool &matches);

		static int interfaceFromLogicalID(Store::LogicalID *lid, TManager::Transaction *tr, Schema *&s);
		static int interfaceFromName(string name, TManager::Transaction *tr, bool &exists, Schema *&s);
	    static void interfaceFromInterfaceNode(const QParser::InterfaceNode *node, Schema *&s);
		static int fromNameIncludingDerivedMembers(string base, Schema *&out, TManager::Transaction *tr, int type);
	    static int completeSupersForBase(string base, vector<Schema *> *&out, TManager::Transaction *tr, int type);	
		static int fromClassVertex(QExecutor::ClassGraphVertex *cgv, Schema *&out);
	    static int viewFromName(string name, TManager::Transaction *tr, bool &exists, Schema *&out);
	    static int viewFromLogicalID(Store::LogicalID *lid, TManager::Transaction *tr, Schema *&out);
	
		static int getCIVObject(string name, TManager::Transaction *tr, bool &exists, int &type, Schema *&out);
		static void getInterfaceForInterfaceOrObjectName(string name, bool &exists, Schema *&out);	
		static int getClassForClassOrObjectName(string name, TManager::Transaction *tr, bool &exists, Schema *&out);
		static int getImplementationForName(string name, TManager::Transaction *tr, bool &exists, int &type, Schema *&out);
	    
	private:
		static int getClassByName(string name, bool &exists, Schema *&out);
		static int getClassByStoreLid(Store::LogicalID *lid, bool &exists, Schema *&out); 
		
	    int m_schemaType;
	    
	    string m_name;
	    string m_associatedObjectName;
	    TFields m_fields;
	    TMethods m_methods;
	    TSupers m_supers;
    };
    
    class Matcher 
    {
	private:
	    static bool matchesAny(Field *intField, TFields impFields);
	    static bool matchesAny(Method *intMethod, TMethods impMethods); 
    
	public:
	    static bool MatchFields(TFields intFields, TFields impFields);
	    static bool MatchMethods(TMethods intMethods, TMethods impMethods);
	    static bool MatchInterfaceWithImplementation(Schema Interface, Schema Implementation);    
    
		static int FindImpBoundToInterface(const string& interfaceObjectName, TManager::Transaction *tr, string& interfaceName, string &impName, string &impObjName, bool &found, int &t);
	};
    

}


#endif	//_INTERFACE_MATCHER_H_

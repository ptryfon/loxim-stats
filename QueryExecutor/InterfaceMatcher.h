#ifndef _INTERFACE_MATCHER_H_
#define _INTERFACE_MATCHER_H_

#include <vector>
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
    typedef vector<string> TSupers;

    class Schema 
    {
	public:
	    Schema(bool isInterfaceSchema = false);
	    ~Schema();
	    TFields getFields() const {return m_fields;}
	    TMethods getMethods() const {return m_methods;}   
	    TSupers getSupers() const {return m_supers;}
	    string getName() const {return m_name;}
	    string getAssociatedObjectName() const {return m_associatedObjectName;}

	    void setIsInterface(bool isInterfaceSchema) {m_isInterfaceSchema = isInterfaceSchema;}
	    void setAssociatedObjectName(string name) {m_associatedObjectName = name;}
	    void setName(string n) {m_name = n;}    
		void setMethods(TMethods m) {m_methods = m;}
		void setFields(TFields f) {m_fields = f;}
		void setSupers(TSupers s) {m_supers = s;}

		void addField(Field *f) {m_fields.push_back(f);}
	    void addMethod(Method *m) {m_methods.push_back(m);}
	    void addSuper(string s) {m_supers.push_back(s);}

	    void debugPrint();
		void printAll(Errors::ErrorConsole *ec) const; 
	    void sortVectors();
	    void join(Schema *s, bool sort = true);

		static int interfaceFromLogicalID(Store::LogicalID *lid, TManager::Transaction *tr, Schema *&s);
	    static void interfaceFromInterfaceNode(const QParser::InterfaceNode *node, Schema *&s);
		static int interfaceMatchesImplementation(string interfaceName, string implementationName, bool &matches);
	    static int fromNameIncludingDerivedMembers(string base, Schema *&out, bool interface);
	    //static Schema* fromQBResult(QExecutor::QueryBagResult *resultBag, QExecutor::QueryExecutor *qE);
	    static int completeSupersForBase(string base, vector<Schema *> *&out, bool interface);	
	    //static int fromName(const string& name, QExecutor::QueryExecutor *qe, Schema *&out);
		static int fromClassVertex(QExecutor::ClassGraphVertex *cgv, Schema *&out);
	    
	private:
	    bool m_isInterfaceSchema;
	    
	    string m_name;
	    string m_associatedObjectName;
	    TFields m_fields;
	    TMethods m_methods;
	    TSupers m_supers;
	    
	    //static int getString(QExecutor::QueryExecutor *qe, QExecutor::QueryResult *qr, string &out);
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
    
	    static int FindClassBoundToInterface(const string& interfaceObjectName, TManager::Transaction *tr, string& interfaceName, string &className, string &invariantName);
	};
    

}


#endif	//_INTERFACE_MATCHER_H_

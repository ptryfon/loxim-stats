#ifndef _INTERFACE_MATCHER_H_
#define _INTERFACE_MATCHER_H_

#include <vector>
#include "QueryResult.h"

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
    
    class QExecutor::QueryExecutor;
    class Schema 
    {
	public:
	    Schema(bool isInterfaceSchema = false);
	    ~Schema();
	    TFields getFields() {return m_fields;}
	    TMethods getMethods() {return m_methods;}   
	    TSupers getSupers() {return m_supers;}
	    void setIsInterface(bool isInterfaceSchema) {m_isInterfaceSchema = isInterfaceSchema;}
	    void addField(Field *f) {m_fields.push_back(f);}
	    void addMethod(Method *m) {m_methods.push_back(m);}
	    void addSuper(string s) {m_supers.push_back(s);}
	    void debugPrint();
	    void sortVectors();
	    void join(Schema *s, bool sort = true);

	    static int interfaceMatchesImplementation(string interfaceName, string implementationName, QExecutor::QueryExecutor *qE, bool &matches);
	    static int fromNameIncludingDerivedMembers(string base, QExecutor::QueryExecutor *qE, Schema *&out);
	    static Schema* fromQBResult(QExecutor::QueryBagResult *resultBag, QExecutor::QueryExecutor *qE);
	    static int completeSupersForBase(string base, QExecutor::QueryExecutor *qE, vector<Schema *> *&out);	
	    
	private:
	    bool m_isInterfaceSchema;
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
    
    };

}


#endif	//_INTERFACE_MATCHER_H_

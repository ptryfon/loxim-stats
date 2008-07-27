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
    
    class Schema 
    {
	public:
	    Schema();
	    ~Schema();
	    TFields getFields() {return m_fields;}
	    TMethods getMethods() {return m_methods;}   
	    void addField(Field *f) {m_fields.push_back(f);}
	    void addMethod(Method *m) {m_methods.push_back(m);}
	    void debugPrint();
	    void sortVectors();
	
	    static Schema* fromQBResult(QExecutor::QueryBagResult resultBag);
	private:
	    bool m_isInterfaceSchema;
	    TFields m_fields;
	    TMethods m_methods;
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

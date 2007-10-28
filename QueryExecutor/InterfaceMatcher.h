#ifndef _INTERFACE_MATCHER_H_
#define _INTERFACE_MATCHER_H_

#include <vector>
#include "QueryResult.h"

typedef string TName;
typedef string TType;
typedef enum TAccess { aPrivate, aPublic, aProtected };

namespace Schemas
{

    class Stringizer 
    {
	public:
	    static string ToStrName(TName name);
	    static string ToStrType(TType type);
	    static string ToStrAccess(TAccess access); 
    };
    
    class Member 
    {
	protected:
	    Member();
	public:
	    TName name;
	    TType type;
	    TAccess access;
    };

    class Field : Member
    {
	public:
	    bool Equals(Field other);
	    bool Matches(Field other);
	    Field(TName n, TType t, TAccess a) {name=n; type=t; access=a;};
    };
    
    class Method : Member
    {
	public:
	    bool Equals(Method other);
	    Method(TName n, TType t, TAccess a, TType r, vector<Member *> p) {name=n;type=t;access=a;
			returnedValueType=r;params=p;paramCount=p.size();};
	    TType returnedValueType;
	protected:
	    vector<Member *> params;
	    int paramCount;
    };

    //any member is unique in terms of (name, paramcount) pair
    class Schema 
    {
	public:
	    Schema(QueryBagResult resultBag); 
	    void AddParent(QueryBagResult parentBag);    
	    vector<Field *> GetFields() {return fields;};
	    vector<Method *> GetMethods() {return methods;}; 
	private:
	    TName name;
	    vector<Field *> fields;
	    vector<Method *> methods;
	    
    };
    
    class MatchResult
    {
	public:
	    enum Result 
	    {
		NotInitialized = -1,
		AllOk,
		NoSuchField,
		AccessMismatch	    
	    };
	    
	    bool isOk();
	    string GetExplanation();
	    MatchResult(Field *f, Result reason) { field = f; matchingResult = reason; };
	    MatchResult() {matchingResult = AllOk;};
	private:
	    Field *field;
	    Result matchingResult;
    };
    
    class Matcher 
    {
	private:
	    static MatchResult MatchFields(vector<Field *> intFields, vector<Field *> impFields);
	    static MatchResult MatchMethods(vector<Method *> intMethods, vector<Method *> impMethods);
	public:
	    static MatchResult MatchInterfaceWithImplementation(Schema Interface, Schema Implementation);
    };

}


#endif	//_INTERFACE_MATCHER_H_

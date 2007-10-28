#include "InterfaceMatcher.h"

using namespace Schemas;

/****************
    Stringizer
****************/

string Stringizer::ToStrName(TName name) 
{
    return name;
};

string Stringizer::ToStrType(TType type)
{
    return type;
};

string Stringizer::ToStrAccess(TAccess access)
{
    switch (access)
    {
	case aPrivate: return "private";
	case aPublic: return "public";
	case aProtected: return "protected";
	default: return "accessNotSupported!";
    };
};


/*************
    Field
*************/

//ADTODO - comparisions!
bool Field::Equals(Field other) 
{
    if ((other.name == name) && (other.type == type) && (other.access == access))
	return true;
    else 
	return false;
};

bool Field::ComparesTo(Field other)
{
    if (other.name == name)
	return true; 
    else 
	return false;
};

bool Field::Matches(Field other)
{
    if ((other.type == type) && (other.access == access))
	return true;
    else 
	return false;
};

string Field::ToString()
{
    string res("");
    res += Stringizer::ToStrAccess(access) + " ";
    res += Stringizer::ToStrName(name) + " ";
    res += Stringizer::ToStrType(type);  
    return res;
};

/*************
    Method
*************/

bool Method::Equals(Method other)
{
    //ADTODO - parameters equal
    return ((other.name == name) && (other.type == type) && (other.access == access)
	&& (other.returnedValueType == returnedValueType) && (other.params == params));
};

/*************
    Schema
*************/

//ADTODO - implement!
Schema::Schema(QueryBagResult qBag)
{
    cout << "SCHEMA CONSTRUCTOR: I am not implemented yet\n";
};

Schema::AddParent(QueryBagResult qBag)
{
    cout << "SCHEMA ADDPARENT: I am not implemented yet\n";
};

/****************
    MatchResult
*****************/

string MatchResult::GetExplanation()
{
    string descr;
    string fieldDescr = "Field " + field->ToString() + " ";
    switch ( matchingResult)
    {
	case NotInitialized:
	    descr = "no matching occured!";
	case AllOk:
	    descr = "matching successful";
	case NoSuchField:
	    descr = fieldDescr + "not matched in implementation";
	case AccessMismatch:
	    descr = fieldDescr + "has mismatch access";
	default:
	    descr = "Error - no explanation!";
	    break;
    };	
};

bool MatchResult::isOk()
{
    if (matchingResult == AllOk)
	return true;
    else 
	return false;    
};

/**************
    Matcher
**************/

//ADTODO - implement
MatchResult Matcher::MatchFields(vector<Field *> intF, vector<Field *> impF)
{
    return new MatchResult();
};

MatchResult Matcher::MatchMethods(vector<Method *> intM, vector<Method *> impM)
{
    return new MatchResult();
};

MatchResult Matcher::MatchInterfaceWithImplementation(Schema interface, Schema implementation)
{
    MatchResult result = MatchFields(interface.GetFields(), implementation.GetFields());
    if (!result.isOk())	return result;
    result = MatchMethods(interface.GetMethods(), implementation.GetMethods());
    return result;
};






#include "InterfaceMatcher.h"
#include "BindNames.h"

using namespace Schemas;
using namespace QExecutor;

namespace
{
    struct AscendingLexFieldsSort
    {
	bool operator() (Field * const& left, Field * const& right)
	{
	    return left->getName() < right->getName();
	}
    };

    struct AscendingMethodSort
    {
	bool operator() (Method * const& left, Method * const& right)
	{
	    string lName = left->getName();
	    string rName = right->getName();
	    if (lName < rName)
		return true;
	    else if (lName > rName)
		return false;
	    else
		return (left->getParamsCount() < right->getParamsCount());
	}
    };
}

/*************
    Method
*************/
void Method::sortParams()
{
    sort(m_params.begin(), m_params.end(), AscendingLexFieldsSort());
    m_paramsSorted = true;
}

bool Method::Matches(Method other) const
{	//both are sorted
    if (m_name != other.getName())
	return false;
	
    int count = getParamsCount();
    if (count != other.getParamsCount()) 
	return false;	
    
    TFields oP = other.getParams();
    TFields::const_iterator thisIt, otherIt;
    for (thisIt = m_params.begin(), otherIt = oP.begin(); thisIt != m_params.end(), otherIt != oP.end(); ++thisIt, ++otherIt)
    {
	Field *f1 = *thisIt;
	Field *f2 = *otherIt;
	if (!f1->Matches(*f2))
	    return false; 
    }
    return true;
}

/*************
    Schema
*************/
void Schema::sortVectors()
{
    sort(m_fields.begin(), m_fields.end(), AscendingLexFieldsSort());
    sort(m_methods.begin(), m_methods.end(), AscendingMethodSort());
}

Schema::Schema() {}

Schema *Schema::fromClassQBResult(QueryBagResult classResultBag)
{	//ADTODO
    return fromInterfaceQBResult(classResultBag);
}

Schema *Schema::fromViewQBResult(QueryBagResult viewResultBag)
{	//ADTODO
    return fromInterfaceQBResult(viewResultBag);
}

Schema *Schema::fromInterfaceQBResult(QueryBagResult interfaceResultBag)
{
    cout << "Schema: fromInterfaceQBResult() \n";
    Schema *s = new Schema();
    QueryResult *res;
    interfaceResultBag.getResult(res);
    QueryStructResult *innerResult = (QueryStructResult *)res;
    vector<QueryResult *> structVec = innerResult->getVector();
    vector<QueryResult *>::iterator it;
    for (it = structVec.begin(); it != structVec.end(); ++it)
    {
	QueryResult *qr = *it;
	QueryBinderResult *qbR = (QueryBinderResult *)qr;
	string name = qbR->getName();
	if (name.compare(QE_OBJECT_NAME_BIND_NAME))
	{
		cout << "OBJECT_NAME_BIND\n";//this does not affect matching
	}
	else if (name.compare(QE_FIELD_BIND_NAME))
	{
	    QueryStructResult *fieldStruct = (QueryStructResult *)qbR->getItem();
	    QueryBinderResult *nameBinder = (QueryBinderResult *)fieldStruct->getVector().front();
	    QueryStringResult *nameString = (QueryStringResult *)nameBinder->getItem();
	    string fieldName = nameString->getValue();
	    Field *f = new Field(fieldName);
	    s->addField(f);		
	    cout << "Field: " << fieldName << "\n"; 	
	}
	else if (name.compare(QE_METHOD_BIND_NAME))
	{
	    QueryStructResult *fieldStruct = (QueryStructResult *)qbR->getItem();
	    vector<QueryResult *> methodInnerBinders = fieldStruct->getVector();
	    vector<QueryResult *>::iterator mIt;
	    Method *m = new Method();
	    for (mIt = methodInnerBinders.begin(); mIt != methodInnerBinders.end(); ++mIt)
	    {
		QueryResult *mQr = *mIt;
		QueryBinderResult *mQbr = (QueryBinderResult *)mQr;
		string mBinderName = mQbr->getName();
		QueryStringResult *nameString = (QueryStringResult *)mQbr->getItem();    
		if (mBinderName.compare(QE_NAME_BIND_NAME))
		{   //method Name
		    m->setName(nameString->getValue());
		    cout << "Method name: " << nameString->getValue() << "\n";
		}
		else if (mBinderName.compare(QE_METHOD_PARAM_BIND_NAME))
		{   //parameter Name
		    Field *f = new Field(nameString->getValue());
		    m->addParam(f);    
		    cout << "Method param name: " << nameString->getValue() << "\n";
		}
	    }
	    s->addMethod(m);
	}
	else
	{
	    cout << "Schema::fromInterfaceQBResult: INVALID result bag!";
	    break;
	}
    }
    
    cout << "Schema: fromInterfaceQBResult(), returning...\n";
    return s;
}

/**************
    Matcher
**************/
//ADTODO - implement
bool Matcher::MatchFields(vector<Field *> intF, vector<Field *> impF)
{
    return false;
}

bool Matcher::MatchMethods(vector<Method *> intM, vector<Method *> impM)
{
    return false;
}

bool Matcher::MatchInterfaceWithImplementation(Schema interface, Schema implementation)
{
    bool result = MatchFields(interface.getFields(), implementation.getFields());
    if (!result) return result;
    result = MatchMethods(interface.getMethods(), implementation.getMethods());
    return result;
}









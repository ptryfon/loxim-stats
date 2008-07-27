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

void Schema::debugPrint()
{
    cout << "Schema:\n";
    string isInt = m_isInterfaceSchema?"yes":"no";
    cout << "\tisInterface = " << isInt << endl;
    cout << "\tFields:\n";
    TFields::iterator itF;
    int i = 0;
    for (itF = m_fields.begin(); itF != m_fields.end(); ++itF)
    {
	i++;
        cout << "\t\t" << i << ". " << (*itF)->getName() << endl;
    }
    cout << "\tMethods:\n";
    TMethods::iterator itM;
    i = 0;
    for (itM = m_methods.begin(); itM != m_methods.end(); ++itM)
    {
	i++;
	Method *m = *itM;
        cout << "\t\t" << i << ". " << m->getName() << endl;
        int j = 0;
        TFields params = m->getParams();
        TFields::iterator itP;
        for (itP = params.begin(); itP != params.end(); ++itP)
        {
    	    j++;
    	    cout << "\t\t\t" << (*itP)->getName() << endl;
        }
    }
}

Schema::Schema() {}

Schema::~Schema() {}

Schema *Schema::fromQBResult(QueryBagResult resultBag)
{
    cout << "Schema: fromQBResult() \n";
    Schema *s = new Schema();
    vector<QueryResult *> structVec = resultBag.getVector();
    vector<QueryResult *>::iterator it;
    for (it = structVec.begin(); it != structVec.end(); ++it)
    {
	QueryResult *qr = *it;
	QueryBinderResult *qbR = (QueryBinderResult *)qr;
	string name = qbR->getName();
	if (!name.compare(QE_FIELD_BIND_NAME))
	{
	    QueryResult *r = qbR->getItem();
	    if (r->type()!=QueryResult::QSTRING) { cout << "ERR"; return s; }
	    QueryStringResult *nameString = (QueryStringResult *)r;    
	    string fieldName = nameString->getValue();
	    Field *f = new Field(fieldName);
	    s->addField(f);		
	    //cout << "Field: " << fieldName << endl; 	
	}
	else if (!name.compare(QE_METHOD_BIND_NAME))
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
		if (!mBinderName.compare(QE_NAME_BIND_NAME))
		{   //method Name
		    m->setName(nameString->getValue());
		    //cout << "Method name: " << nameString->getValue() << "\n";
		}
		else if (!mBinderName.compare(QE_METHOD_PARAM_BIND_NAME))
		{   //parameter Name
		    Field *f = new Field(nameString->getValue());
		    m->addParam(f);    
		    //cout << "Method param name: " << nameString->getValue() << "\n";
		}
	    }
	    s->addMethod(m);
	    //cout << "Method: " << m->getName() << "\n";
	}
    }
    s->debugPrint();
    //cout << "Schema: fromQBResult(), returning...\n";
    return s;
}

/**************
    Matcher
**************/

bool Matcher::matchesAny(Field *f, TFields impF)
{
    TFields::iterator it;
    for (it = impF.begin(); it != impF.end(); ++it)
    {
	Field *cand = (*it);
	if (f->Matches(*cand))
	    return true;
    }
    return false;
}

bool Matcher::matchesAny(Method *m, TMethods impM)
{
    TMethods::iterator it;
    m->sortParams();
    for (it = impM.begin(); it != impM.end(); ++it)
    {
	Method *cand = (*it);
	cand->sortParams();
	if (m->Matches(*cand))
	    return true;
    }
    return false;
}

bool Matcher::MatchFields(TFields intF, TFields impF)
{
    TFields::iterator intI;
    for (intI = intF.begin(); intI != intF.end(); ++intI)
    {
	Field *intField = (*intI);
	if (!Matcher::matchesAny(intField, impF))
	    return false;
    }
    return true;
}


bool Matcher::MatchMethods(TMethods intM, TMethods impM)
{
    TMethods::iterator intI;
    for (intI = intM.begin(); intI != intM.end(); ++intI)
    {
	Method *intMethod = (*intI);
	if (!Matcher::matchesAny(intMethod, impM))
	    return false;
    }
    return true;
}

bool Matcher::MatchInterfaceWithImplementation(Schema interface, Schema implementation)
{
    interface.sortVectors();
    implementation.sortVectors();
    
    bool result = MatchFields(interface.getFields(), implementation.getFields());
    if (!result) {cout << "Fields mismatch" << endl; return result;}
    result = MatchMethods(interface.getMethods(), implementation.getMethods());
    if (!result) cout << "Methods mismatch" << endl; else cout << "Match!" << endl;
    return result;
}









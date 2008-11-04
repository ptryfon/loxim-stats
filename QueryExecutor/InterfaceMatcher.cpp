#include "InterfaceMatcher.h"
#include "BindNames.h"
#include "QueryExecutor.h"
#include "InterfaceQuery.h"
#include <set>

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
	bool operator() (Schemas::Method * const& left, Schemas::Method * const& right)
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
    
    template <class T>
    std::set<T> vectorToSet(std::vector<T> vec)
    {
	std::set<T> retSet;
	while (!vec.empty())
	{
	    T t(vec.back());
	    retSet.insert(t);
	    vec.pop_back();
	}
	return retSet;
    };
}

/*************
    Method
*************/
void Schemas::Method::sortParams()
{
    sort(m_params.begin(), m_params.end(), AscendingLexFieldsSort());
    m_paramsSorted = true;
}

bool Schemas::Method::Matches(Schemas::Method other) const
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

Schema::Schema(bool isInterfaceSchema) : m_isInterfaceSchema(isInterfaceSchema) {}

Schema::~Schema() {}

int Schema::getString(QueryExecutor *qe, QueryResult *r, string &out)
{
    QueryResult *tmpR;
    qe->derefQuery(r, tmpR);
    if (tmpR->type()!=QueryResult::QSTRING) { cout << "ERR"; return -1; }
    QueryStringResult *nameString = (QueryStringResult *)tmpR;    
    out = nameString->getValue();
    return 0;
}

Schema *Schema::fromQBResult(QueryBagResult *resultBag, QueryExecutor *qe)
{
    cout << "Schema: fromQBResult() \n";
    bool isInterface = false;
    Schema *s = new Schema();
    QueryResult *tmp;
    string generalNameString;
    qe->derefQuery(resultBag, tmp);
    QueryBagResult *derefedBag = (QueryBagResult *)tmp;
    
    vector<QueryResult *> structVec = derefedBag->getVector();
    vector<QueryResult *>::iterator it;
    for (it = structVec.begin(); it != structVec.end(); ++it)
    {
	QueryResult *qr = *it;
	QueryBinderResult *qbR = (QueryBinderResult *)qr;
	string name = qbR->getName();
	if (!name.compare(QE_OBJECT_NAME_BIND_NAME))
	{
	    isInterface = true;
	    if (Schema::getString(qe, qbR->getItem(), generalNameString) != 0) return s;
	    s->setAssociatedObjectName(generalNameString);
	}
	else if (!name.compare(QE_INVARIANT_BIND_NAME))
	{
	    if (Schema::getString(qe, qbR->getItem(), generalNameString) != 0) return s;
	    s->setAssociatedObjectName(generalNameString);
	}
	else if (!name.compare(QE_FIELD_BIND_NAME))
	{
	    if (Schema::getString(qe, qbR->getItem(), generalNameString) != 0) return s;
	    Field *f = new Field(generalNameString);
	    s->addField(f);		
	    //cout << "Field: " << generalNameString << endl; 	
	}
	else if (!name.compare(QE_EXTEND_BIND_NAME))
	{
	    if (Schema::getString(qe, qbR->getItem(), generalNameString) != 0) return s;
	    s->addSuper(generalNameString);		
	    //cout << "Super: " << generalNameString << endl; 	
	}
	else if (!name.compare(QE_METHOD_BIND_NAME))
	{
	    QueryResult *methodResult = qbR->getItem();
	    
	    //Get method name
	    QueryReferenceResult *ref = (QueryReferenceResult *)qbR->getItem();
	    LogicalID *methodLid = ref->getValue();
	    ObjectPointer *optr;
	    if ((qe->tr->getObjectPointer(methodLid, Store::Read, optr, false) != 0) || (!optr)) { cout << "ERR"; return s;}
	    string name = optr->getName();
	    Schemas::Method *m = new Schemas::Method();
	    m->setName(name);
	    
	    //Dereference to see parameters
	    QueryResult *tmpR;
	    qe->derefQuery(methodResult, tmpR);
	    if ((!tmpR) || (tmpR->type()!=QueryResult::QSTRUCT)) { cout << "ERR"; return s; }
	    QueryStructResult *fieldStruct = (QueryStructResult *)tmpR;
	    
	    //Process parameters
	    vector<QueryResult *> methodInnerBinders = fieldStruct->getVector();
	    vector<QueryResult *>::iterator mIt;
	    for (mIt = methodInnerBinders.begin(); mIt != methodInnerBinders.end(); ++mIt)
	    {
		QueryResult *mQr = *mIt;
		QueryBinderResult *mQbr = (QueryBinderResult *)mQr;
		string mBinderName = mQbr->getName();
		QueryStringResult *nameString = (QueryStringResult *)mQbr->getItem();    
		if (!mBinderName.compare(QE_METHOD_PARAM_BIND_NAME))
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
    s->setIsInterface(isInterface);
    s->debugPrint();
    return s;
}

int Schema::interfaceMatchesImplementation(string interface, string implementation, QueryExecutor *qe, bool &out)
{
    Schema *intSchema;
    Schema *impSchema;
    int errorcode = fromNameIncludingDerivedMembers(interface, qe, intSchema);
    if (errorcode) return errorcode;
    
    errorcode = fromNameIncludingDerivedMembers(implementation, qe, impSchema);
    if (errorcode) return errorcode;
    
    out = Matcher::MatchInterfaceWithImplementation(*intSchema, *impSchema);
    delete intSchema;
    delete impSchema;
    return 0;
}

int Schema::fromNameIncludingDerivedMembers(string name, QueryExecutor *qe, Schema *&out)
{
    vector<Schema *> *allSchemas;
    int errorcode = completeSupersForBase(name, qe, allSchemas);
    if (errorcode) return errorcode;
    if (allSchemas->size() < 1) return -1; //TODO
    out = allSchemas->back();
    string objectName = allSchemas->front()->getAssociatedObjectName();
    allSchemas->pop_back();
    vector<Schema *>::iterator it;
    for (it = allSchemas->begin(); it != allSchemas->end(); ++it)
    {
	Schema *toJoin = *it;
	out->join(toJoin);
    }
    out->setAssociatedObjectName(objectName);
    cout << "fromNameIncludingDerivedMembers returning" << endl;
    return 0;
}

int Schema::completeSupersForBase(string baseString, QueryExecutor *qe, vector<Schema *> *&out)
{
    TSupers base;
    base.push_back(baseString);
    out = new vector<Schema *>(0);
    set<string> baseSet = vectorToSet<string>(base);
    TSupers::iterator it;
    int errorcode;
    while (!base.empty())
    {
	string name = base.back();
	base.pop_back();
	NameNode nameN(name);
	errorcode = qe->executeRecQuery(&nameN);
	if (errorcode != 0)
	{	//TODO
	    cout << "ERROR";
	    return errorcode;
	}
	QueryResult *res;
	errorcode = qe->qres->pop(res);
	if ((errorcode != 0) || (res->type() != QueryResult::QBAG)) 
	{	//TODO
	    cout << "ERROR";
	    return errorcode;
	}
	QueryBagResult *superBag = dynamic_cast<QueryBagResult *>(res);
	Schema *s = fromQBResult(superBag, qe);
	TSupers baseAddition = s->getSupers();
	TSupers::iterator addIt;
	for (addIt = baseAddition.begin(); addIt != baseAddition.end(); ++addIt)
	{
	    string superName = *addIt;
	    if (baseSet.find(superName) == baseSet.end())
	    {
		baseSet.insert(superName);
		base.push_back(superName);
	    }
	    //else - wielodziedziczenie, petla..?
	}
	out->push_back(s);
    }
    return 0;
}


void Schema::join(Schema *s, bool sort)
{	//Discards supers
    m_supers.clear();
    set<Field *> hostFields = vectorToSet<Field*>(m_fields);
    set<Method *> hostMethods = vectorToSet<Method*>(m_methods);
    TFields guestFields = s->getFields();
    TMethods guestMethods = s->getMethods();
    TFields::iterator itF;
    for (itF = guestFields.begin(); itF != guestFields.end(); ++itF)
    {
	Field *f = new Field(**itF);
	if (hostFields.find(f) == hostFields.end())
	{
	    hostFields.insert(f);
	    addField(f);
	}
    }
    
    TMethods::iterator itM;
    for (itM = guestMethods.begin(); itM != guestMethods.end(); ++itM)
    {
	Method *m = new Method(**itM);
	if (hostMethods.find(m) == hostMethods.end())
	{
	    hostMethods.insert(m);
	    addMethod(m);
	}
    }
    if (sort) sortVectors();
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

bool Matcher::matchesAny(Schemas::Method *m, TMethods impM)
{
    TMethods::iterator it;
    m->sortParams();
    for (it = impM.begin(); it != impM.end(); ++it)
    {
	Schemas::Method *cand = (*it);
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
	Schemas::Method *intMethod = (*intI);
	if (!Matcher::matchesAny(intMethod, impM))
	    return false;
    }
    return true;
}

bool Matcher::MatchInterfaceWithImplementation(Schema interface, Schema implementation)
{
    interface.sortVectors();
    implementation.sortVectors();
    
    interface.debugPrint();
    implementation.debugPrint();
    
    bool result = MatchFields(interface.getFields(), implementation.getFields());
    if (!result) {cout << "Fields mismatch" << endl; return result;}
    result = MatchMethods(interface.getMethods(), implementation.getMethods());
    if (!result) cout << "Methods mismatch" << endl; else cout << "Match!" << endl;
    return result;
}

int Matcher::QueryForInterfaceObjectName(const string& name, QueryExecutor *qe, Errors::ErrorConsole *ec, TManager::Transaction *tr, QueryNode *&query)
{
    string iName, bName;
    int errcode = tr->getInterfaceBindForObjectName(name, iName, bName);
    if (errcode)
    {
	return errcode;
    }
    else
    {
	if (bName.empty())
	{	//TODO - jest to nazwa obiektow spelniajacych interfejs, ale interfejs jest nie zwiazany z implementacja
	    ec->printf("[QE] interface not bound\n");
	    return 0;
	}
	else
	{
	    ec->printf("[QE] found implementation (%s -> %s) \n", iName.c_str(), bName.c_str());
	    Schema *intSchema;
	    errcode = Schema::fromNameIncludingDerivedMembers(iName, qe, intSchema);
	    if (errcode)
	    {	//TODO
		ec->printf("[QE] error while creating schema for interface name: %s\n", iName.c_str());
		return errcode;
	    }
	    Schema *impSchema;
	    errcode = Schema::fromNameIncludingDerivedMembers(bName, qe, impSchema);
	    if (errcode)
	    {	//TODO
		ec->printf("[QE] error while creating schema for implementation name: %s\n", bName.c_str());
		return errcode;
	    }
	    
	    intSchema->setIsInterface(true);
	    impSchema->setIsInterface(false);
	    string classInvariantName = impSchema->getAssociatedObjectName();
	    if (classInvariantName.empty())
	    {	//TODO - no inva!
		ec->printf("[QE] no invariant name specified!\n");
		return 0;
	    }
	    ec->printf("[QE] associated object name - %s\n", classInvariantName.c_str());
	    
	    InterfaceQuery::produceQuery(intSchema, ec, classInvariantName, query);
	}
    }
    return 0;
}







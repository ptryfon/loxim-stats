#include "InterfaceMatcher.h"
#include "InterfaceMaps.h"
#include "BindNames.h"
#include "QueryExecutor.h"
#include "InterfaceQuery.h"
#include "ClassGraph.h"
#include "QueryResult.h"
#include <set>

using namespace Schemas;

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
		retSet.insert(vec.begin(), vec.end());
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

void Schema::printAll(Errors::ErrorConsole *ec) const
{
	ec->printf("Schema for %s\n", m_name.c_str());
	string isInterface = m_isInterfaceSchema?"true":"false";
	ec->printf("\tinterface = %s\n", isInterface.c_str());
	ec->printf("\tFields:\n");
	int i = 0;
	TFields::const_iterator itF;
	for (itF = m_fields.begin(); itF != m_fields.end(); ++itF)
    {
		i++;
		const Field *f = *itF;
        ec->printf("\t\t%d. %s\n", i, f->getName().c_str());
    }
	ec->printf("\tMethods:\n");
    TMethods::const_iterator itM;
    i = 0;
    for (itM = m_methods.begin(); itM != m_methods.end(); ++itM)
    {
		i++;
		const Method *m = *itM;
        ec->printf("\t\t%d. %s\n", i, m->getName().c_str());
        int j = 0;
        TFields params = m->getParams();
        TFields::iterator itP;
        for (itP = params.begin(); itP != params.end(); ++itP)
        {
    	    j++;
    	    ec->printf("\t\t\t %d. %s\n", j, (*itP)->getName().c_str());
        }
    }
}

Schema::Schema(bool isInterfaceSchema) : m_isInterfaceSchema(isInterfaceSchema) {}

Schema::~Schema() {}

/*
int Schema::getString(QExecutor::QueryExecutor *qe, QueryResult *r, string &out)
{
    QueryResult *tmpR;
    qe->derefQuery(r, tmpR);
    if (tmpR->type()!=QueryResult::QSTRING) { return -1; }
    QueryStringResult *nameString = (QueryStringResult *)tmpR;    
    out = nameString->getValue();
    return 0;
}

//Get rid of this
Schema *Schema::fromQBResult(QueryBagResult *resultBag, QueryExecutor *qe)
{
    //cout << "Schema: fromQBResult() \n";
    bool isInterface = false;
    Schema *s = new Schema();
    QueryResult *tmp;
    string generalNameString;
    qe->derefQuery(resultBag, tmp);
    QueryBagResult *derefedBag = (QueryBagResult *)tmp;
    
	//TODO - name
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
			if ((qe->tr->getObjectPointer(methodLid, Store::Read, optr, false) != 0) || (!optr)) { return s;}
			string name = optr->getName();
			Schemas::Method *m = new Schemas::Method();
			m->setName(name);
			
			//Dereference to see parameters
			QueryResult *tmpR;
			qe->derefQuery(methodResult, tmpR);
			if ((!tmpR) || (tmpR->type()!=QueryResult::QSTRUCT)) { return s;}
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
    return s;
}
*/

void Schema::interfaceFromInterfaceNode(const QParser::InterfaceNode *node, Schema *&s)
{
	s->setName(node->getInterfaceName());
	s->setAssociatedObjectName(node->getObjectName());
	QParser::TAttributes attribs = node->getAttributes();
	QParser::TAttributes::iterator it;
	for (it = attribs.begin(); it != attribs.end(); ++it)
	{
		InterfaceAttribute a = *it;
		Field *f = new Field(a.getName());
		s->addField(f);
	}
	
	QParser::TMethods methods = node->getMethods();
	QParser::TMethods::iterator itM;
	for (itM = methods.begin(); itM != methods.end(); ++itM)
	{
		InterfaceMethod m = *itM;
		Method *meth = new Method(m.getName());
		QParser::TAttributes params = m.getParams();
		QParser::TAttributes::iterator itP;
		for (itP = params.begin(); itP != params.end(); ++itP)
		{
			InterfaceAttribute a = *itP;
			Field *f = new Field(a.getName());
			meth->addParam(f);
		}
		meth->sortParams();
		s->addMethod(meth);
	}
		
	TSupers supers;
	QParser::TSupers sups = node->getSupers();
	supers.insert(supers.begin(), sups.begin(), sups.end());
	s->setSupers(supers);
	s->setIsInterface(true);
}

int Schema::interfaceFromLogicalID(LogicalID *lid, TManager::Transaction *tr, Schema *&s)
{
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(lid, Store::Read, optr, false);
	if (errcode != 0)
		return errcode;
	//cout << "+\n";
	DataValue* dv = optr->getValue();
	s->setName(optr->getName());
	//cout << dv->getSubtype();
	//if(dv->getSubtype() != Store::Interface) return -1;
	//cout << "+\n";
	
	vector<LogicalID*>* lidVec = dv->getVector();
	vector<LogicalID*>::iterator it;
	for (it = lidVec->begin(); it!=lidVec->end(); ++it) 
	{
		LogicalID* currentLid = *it;
		ObjectPointer *optrInside;
		errcode = tr->getObjectPointer (currentLid, Store::Read, optrInside, false);
		if (errcode != 0)
			return errcode;
		
		DataValue *dvInside = optrInside->getValue();

		if (optrInside->getName() == QE_OBJECT_NAME_BIND_NAME) 
		{
			string objectName = dvInside->getString();
			//cout << "object NAME: " << objectName << endl;
			s->setAssociatedObjectName(objectName);
		}
		else if (optrInside->getName() == QE_METHOD_BIND_NAME) 
		{		
			LogicalID *methLid = dvInside->getPointer();
			ObjectPointer *optrMeth;
			errcode = tr->getObjectPointer (methLid, Store::Read, optrMeth, false);
			if (errcode != 0)
				return errcode;

			DataValue* dvMeth = optrMeth->getValue();
			string methName = optrMeth->getName();
			//cout << "Method name: " << methName << endl;
			Schemas::Method *m = new Schemas::Method();
			m->setName(methName);
			
			vector<LogicalID*>* methLids = dvMeth->getVector();
			vector<LogicalID*>::iterator mIt;
			for (mIt = methLids->begin(); mIt!=methLids->end(); ++mIt)
			{
				LogicalID* iLid = *mIt;
				ObjectPointer *optrMethInside;
				errcode = tr->getObjectPointer(iLid, Store::Read, optrMethInside, false);
				if (errcode)
					return errcode;
				DataValue *dvMethInside = optrMethInside->getValue();
				string memberName = optrMethInside->getName();
				if (memberName == QE_METHOD_PARAM_BIND_NAME)
				{
					string fieldName = dvMethInside->getString();
					//cout << "Method param name: " << fieldName << endl;
					Field *f = new Field(fieldName);
					m->addParam(f);    
				}
			}
			s->addMethod(m);
		}
		else if (optrInside->getName() == QE_FIELD_BIND_NAME) 
		{
			string fieldName;
			if (dvInside->getType() == Store::Pointer)
			{
				LogicalID *fieldLid = dvInside->getPointer();
				ObjectPointer *optrField;
				errcode = tr->getObjectPointer (fieldLid, Store::Read, optrField, false);
				if (errcode != 0)
					return errcode;
				DataValue* dvField = optrField->getValue();
				fieldName = dvField->getString();
			}
			else
				fieldName = dvInside->getString();

			//cout << "FIELD NAME: " << fieldName << endl;
			Field *f = new Field(fieldName);
			s->addField(f);		
		}
		else if (optrInside->getName() == QE_EXTEND_BIND_NAME) 
		{
			string extendName = dvInside->getString();
			//cout << "Extend NAME: " << extendName << endl;
			s->addSuper(extendName);
		}
	}
	s->setIsInterface(true);
	return 0;
}

int Schema::interfaceMatchesImplementation(string interface, string implementation, bool &out)
{
    Schema *intSchema;
    Schema *impSchema;
    int errorcode = fromNameIncludingDerivedMembers(interface, intSchema, true);
    if (errorcode) return errorcode;
    
    errorcode = fromNameIncludingDerivedMembers(implementation, impSchema, false);
    if (errorcode) return errorcode;
    
    out = Matcher::MatchInterfaceWithImplementation(*intSchema, *impSchema);
    delete intSchema;
    delete impSchema;
    return 0;
}

int Schema::fromNameIncludingDerivedMembers(string name, Schema *&out, bool interface)
{
    vector<Schema *> *allSchemas;
    int errorcode = completeSupersForBase(name, allSchemas, interface);
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
    //cout << "fromNameIncludingDerivedMembers returning" << endl;
    return 0;
}

/*
int Schema::fromName(const string& name, QueryExecutor *qe, Schema *&out)
{
	NameNode nameN(name);
	int errorcode = qe->executeRecQuery(&nameN);
	if (errorcode != 0)
	{	//TODO
	    return errorcode;
	}
	QueryResult *res;
	errorcode = qe->qres->pop(res);
	if ((errorcode != 0) || (res->type() != QueryResult::QBAG)) 
	{   //TODO
	    return errorcode;
	}
	
	QueryBagResult *qBag = dynamic_cast<QueryBagResult *>(res);
	out = fromQBResult(qBag, qe);
	return 0;
}
*/

int Schema::fromClassVertex(QExecutor::ClassGraphVertex *cgv, Schema *&out)
{
	out = new Schema();
	out->setIsInterface(false);
	out->setName(cgv->name);
	out->setAssociatedObjectName(cgv->invariant);
	QExecutor::ClassGraph *cg;
	int errcode = QExecutor::ClassGraph::getHandle(cg);
	if (errcode) return errcode; //could not get handle
	
	SetOfLids extends = cgv->extends;
	for (SetOfLids::iterator it = extends.begin(); it != extends.end(); ++it)
	{
		QExecutor::ClassGraphVertex *cgvExt;
		errcode = cg->getVertex(*it, cgvExt);
		if (errcode) return errcode; //corrupted graph..
		string extName = cgvExt->name;
		out->addSuper(extName);		
	}
	
	QExecutor::stringHashSet fields = cgv->fields;
	for (QExecutor::stringHashSet::iterator it = fields.begin(); it != fields.end(); ++it)
	{
		Field *f = new Field(*it);
		out->addField(f);		
	}
	
	QExecutor::NameToArgCountToMethodMap methods = cgv->methods;
	for(QExecutor::NameToArgCountToMethodMap::iterator it = methods.begin(); it != methods.end(); ++it) 
	{
		string methodName = (*it).first;
		QExecutor::ArgsCountToMethodMap* map = (*it).second;
		for(QExecutor::ArgsCountToMethodMap::iterator j = map->begin(); j != map->end(); ++j) 
		{
			Schemas::Method *method = new Method(methodName);
			QExecutor::Method *m = (*j).second;
			vector<string> params = m->params;
			for(vector<string>::iterator p = params.begin(); p != params.end(); ++p)
			{
				string param = *p;
				Field *f = new Field(param);
				method->addParam(f);
			}
			out->addMethod(method);
		}
	}
	return 0;
}

int Schema::completeSupersForBase(string baseString, vector<Schema *> *&out, bool interface)
{
    TSupers base;
    base.push_back(baseString);
    out = new vector<Schema *>(0);
    set<string> baseSet = vectorToSet<string>(base);
    TSupers::iterator it;
    int errcode;
    while (!base.empty())
    {
		string name = base.back();
		base.pop_back();
		Schema *s;
		if (interface)
		{		
			if (!InterfaceMaps::Instance().hasInterface(name))
				return -1;  //corrupted map..
		
			s = new Schema();
			*s = InterfaceMaps::Instance().getSchemaForInterface(name);
		}
		else
		{
			QExecutor::ClassGraph *cg;
			errcode = QExecutor::ClassGraph::getHandle(cg);
			if (errcode) return errcode; //could'nt get handle
			QExecutor::ClassGraphVertex *cgv; 
			
			bool fExists;
			cg->getClassVertexByName(name, cgv, fExists);
			if (!fExists)
				return -1;  //corrupted graph..
			
			errcode = fromClassVertex(cgv, s);
			if (errcode) return errcode;
		}
		
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
    
    bool result = MatchFields(interface.getFields(), implementation.getFields());
    if (!result) {return result;}
    result = MatchMethods(interface.getMethods(), implementation.getMethods());
    return result;
}

int Matcher::FindClassBoundToInterface(const string& interfaceObjectName, TManager::Transaction *tr, string& interfaceName, string &className, string &invariantName)
{
    int errcode = tr->getInterfaceBindForObjectName(interfaceObjectName, interfaceName, className);
    if (interfaceName.empty())
    {   //TODO - nie ma takiego interfejsu (interfaceObjectName nie jest nazwa obiektu zadnego interfejsu)
		return -1;
	}
	else if (errcode)
	{
		return errcode;
    }
    else
    {
		if (className.empty())
		{	//TODO - jest to nazwa obiektow spelniajacych interfejs, ale interfejs jest nie zwiazany z implementacja
			return -1;
		}
		else 
		{
			QExecutor::ClassGraph *cg;
			QExecutor::ClassGraph::getHandle(cg);
			QExecutor::ClassGraphVertex *cgv;
			bool fExists;
			cg->getClassVertexByName(className, cgv, fExists);
			if (!fExists)
				return -1;  //TODO - interfejs zwiazany z nieistniejaca klasa
			invariantName = cgv->invariant;
		}
	}
	return 0;
}







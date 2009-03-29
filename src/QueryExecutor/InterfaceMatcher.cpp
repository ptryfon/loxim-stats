#include <QueryExecutor/InterfaceMatcher.h>
#include <QueryExecutor/InterfaceMaps.h>
#include <QueryExecutor/BindNames.h>
#include <QueryExecutor/QueryExecutor.h>
#include <QueryExecutor/InterfaceQuery.h>
#include <QueryExecutor/ClassGraph.h>
#include <QueryExecutor/QueryResult.h>
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
	info_printf(*ec, "Schema for %s\n", m_name.c_str());
	info_printf(*ec, "\tFields:\n");
	int i = 0;
	TFields::const_iterator itF;
	for (itF = m_fields.begin(); itF != m_fields.end(); ++itF)
    {
		i++;
		const Field *f = *itF;
        info_printf(*ec, "\t\t%d. %s\n", i, f->getName().c_str());
    }
	info_printf(*ec, "\tMethods:\n");
    TMethods::const_iterator itM;
    i = 0;
    for (itM = m_methods.begin(); itM != m_methods.end(); ++itM)
    {
		i++;
		const Method *m = *itM;
        info_printf(*ec, "\t\t%d. %s\n", i, m->getName().c_str());
        int j = 0;
        TFields params = m->getParams();
        TFields::iterator itP;
        for (itP = params.begin(); itP != params.end(); ++itP)
        {
    	    j++;
    	    info_printf(*ec, "\t\t\t %d. %s\n", j, (*itP)->getName().c_str());
        }
    }
}

string Schema::toSchemaString() const
{
	string out;
	string instance = "instance ";
	string proc;
	if (m_schemaType != BIND_INTERFACE)
		return "";

	out += "interface " + m_name;
	out += "\n{\n\t";
	out += "instance " + m_associatedObjectName + ":\n\t{";
	
	int i = 0;
	for (TFields::const_iterator fit = m_fields.begin(); fit != m_fields.end(); ++fit)
	{
		if (i > 0) out += ";";
		out += "\n\t\t";
		const Field *f = *fit;
		out += f->getName();
		i++;
	}
		
	out += "\n\t}";
	i = 0;
	for (TMethods::const_iterator mit = m_methods.begin(); mit != m_methods.end(); ++mit)
	{
		if (i > 0) out += ";";
		out += "\n\t";
		const Method *m = *mit;
		out += proc;
		out += m->getName();
		out += "(";
		TFields params = m->getParams();
		for (TFields::const_iterator it = params.begin(); it != params.end(); ++it)
		{
			const Field *p = *it;
			out += p->getName();
			if ((it + 1) != params.end())
				out += ", ";
		}
		out += ")\n";
		i++;
	}
		
		
	out += "\n}";
	
	//out += m_associatedObjectName;
	//out += ";\n";
	return out;
}

Schema::Schema(int type) : m_schemaType(type) {}

Schema::~Schema() {}

void Schema::interfaceFromInterfaceNode(const QParser::InterfaceNode *node, Schema *&s)
{
	s = new Schema();
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
		
	QParser::TSupers sups = node->getSupers();
	TSupers supers(sups.begin(), sups.end());
	s->setSupers(supers);
	s->setSchemaType(BIND_INTERFACE);
}

int Schema::interfaceFromLogicalID(LogicalID *lid, TManager::Transaction *tr, Schema *&s)
{
	s = new Schema();
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(lid, Store::Read, optr, false);
	if (errcode != 0)
		return errcode;
	DataValue* dv = optr->getValue();
	s->setName(optr->getName());
	if(dv->getSubtype() != Store::Interface)
		return (ErrQExecutor | EOtherResExp);
	
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

			Field *f = new Field(fieldName);
			s->addField(f);		
		}
		else if (optrInside->getName() == QE_EXTEND_BIND_NAME) 
		{
			string extendName = dvInside->getString();
			s->addSuper(extendName);
		}
	}
	s->setSchemaType(BIND_INTERFACE);
	return 0;
}

int Schema::viewFromLogicalID(LogicalID *lid, TManager::Transaction *tr, Schema *&s)
{
	s = new Schema();
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(lid, Store::Read, optr, false);
	if (errcode != 0)
		return errcode;
	DataValue* dv = optr->getValue();
	s->setName(optr->getName());
	if(dv->getSubtype() != Store::View)
		return (ErrQExecutor | EOtherResExp);
	
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

		if (optrInside->getName() == QE_VIEWDEF_VIRTUALOBJECTS_NAME) 
		{
			string objectName = dvInside->getString();
			s->setAssociatedObjectName(objectName);
		}
	}
	s->setSchemaType(BIND_VIEW);
	return 0;
}

int Schema::interfaceFromName(string name, TManager::Transaction *tr, bool &exists, Schema *&s)
{
	exists = false;
	vector<LogicalID*>* interfacesLids;
	int errcode = tr->getInterfacesLID(name, interfacesLids);
	if (errcode)
	{
		return errcode;
	}
	if (interfacesLids->size() == 0) return 0;
	exists = true;
	
	return interfaceFromLogicalID(interfacesLids->at(0), tr, s);	
}

int Schema::viewFromName(string name, TManager::Transaction *tr, bool &exists, Schema *&out)
{
	exists = false;
	vector<LogicalID*>* viewsLids;
	int errcode = tr->getViewsLID(name, viewsLids);
	if (errcode) 
	{
		return errcode;
	}
	if (viewsLids->size() == 0) return 0;
	exists = true;
	
	return viewFromLogicalID(viewsLids->at(0), tr, out);	
}

int Schema::interfaceMatchesImplementation(string interface, string implementation, TManager::Transaction *tr, int t, bool &out)
{
    Schema *intSchema;
    Schema *impSchema;
	
	ErrorConsole *ec = &ErrorConsole::get_instance(EC_OUTER_SCHEMAS);
    int errorcode = fromNameIncludingDerivedMembers(interface, intSchema, tr, BIND_INTERFACE);
    if (errorcode) return errorcode;
    
    errorcode = fromNameIncludingDerivedMembers(implementation, impSchema, tr, t);
    if (errorcode) return errorcode;
    
    out = Matcher::MatchInterfaceWithImplementation(*intSchema, *impSchema);
    
	delete intSchema;
    delete impSchema;
    return 0;
}

int Schema::fromNameIncludingDerivedMembers(string name, Schema *&out, TManager::Transaction *tr, int t)
{
    vector<Schema *> *allSchemas;
    int errorcode = completeSupersForBase(name, allSchemas, tr, t);
    if (errorcode) return errorcode;
    if (allSchemas->size() < 1) 
		return (ErrQExecutor | EOtherResExp);
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
    return 0;
}

int Schema::fromClassVertex(QExecutor::ClassGraphVertex *cgv, Schema *&out)
{
	out = new Schema();
	out->setSchemaType(BIND_CLASS);
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

int Schema::completeSupersForBase(string baseString, vector<Schema *> *&out, TManager::Transaction *tr, int t)
{
    out = new vector<Schema *>(0);
    vector<string> base;
	base.push_back(baseString);
	TSupers baseSet;
    baseSet.insert(baseString);
	
    int errcode;
    while (!base.empty())
    {
		string name = base.back();
		base.pop_back();
		Schema *s;
		switch(t)
		{
			case BIND_INTERFACE:
			{		
				if (!InterfaceMaps::Instance().hasInterface(name))
					return (ErrQExecutor | EInterfaceMapCorrupted); 
		
				s = new Schema();
				*s = InterfaceMaps::Instance().getSchemaForInterface(name);
				break;
			}
			case BIND_CLASS:
			{
				QExecutor::ClassGraph *cg;
				errcode = QExecutor::ClassGraph::getHandle(cg);
				if (errcode) return errcode; //could'nt get handle
				QExecutor::ClassGraphVertex *cgv; 
			
				bool fExists;
				cg->getClassVertexByName(name, cgv, fExists);
				if (!fExists)
					return (ErrQExecutor | EClassGraphCorrupted);  //corrupted graph..
			
				errcode = fromClassVertex(cgv, s);
				if (errcode) return errcode;
				break;
			}
			case BIND_VIEW:
			{
				bool exists;
				errcode = viewFromName(name, tr, exists, s);
				if (errcode || !exists) return errcode;
				break;
			}
			default:
				return (ErrQExecutor | EOtherResExp);
				break;
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

//Class, Interface or View
int Schema::getCIVObject(string name, TManager::Transaction *tr, bool &exists, int &t, Schema *&out)
{
	exists = false;
	int errcode = Schema::interfaceFromName(name, tr, exists, out);
	if (errcode) return errcode;
	if (exists)
	{
		t = BIND_INTERFACE;
		return 0;   //interface
	}
	
	errcode = getClassByName(name, exists, out);
	if (errcode) return errcode;
	if (exists)
	{
		t = BIND_CLASS;
		return 0; //class
	}
	
	errcode = viewFromName(name, tr, exists, out);
	if (errcode) return errcode;
	if (exists)
	{
		t = BIND_VIEW;
		return 0;   //view
	}
	
	return 0;
}

int Schema::getClassByName(string name, bool &exists, Schema *&out)
{
	exists = false;
	QExecutor::ClassGraph *cg;
	int errcode = QExecutor::ClassGraph::getHandle(cg);
	if (errcode)
		return errcode;

	QExecutor::ClassGraphVertex *cgv;
	cg->getClassVertexByName(name, cgv, exists);
	if (exists)
	{
		out = NULL;
		errcode = fromClassVertex(cgv, out);
		if (errcode) return errcode; //graph corrupted
		return 0;
	}
	return 0;
}

int Schema::getClassByStoreLid(LogicalID *lid, bool &exists, Schema *&out)
{
	exists = false;
	QExecutor::ClassGraph *cg;
	int errcode = QExecutor::ClassGraph::getHandle(cg);
	if (errcode)
		return errcode;

	LogicalID *cgLid;
	errcode = cg->lidToClassGraphLid(lid, cgLid);
	if (errcode) return errcode;

	QExecutor::ClassGraphVertex *cgv;
	if (!cg->vertexExist(cgLid)) return 0;
	cg->getVertex(cgLid, cgv);
	errcode = fromClassVertex(cgv, out);
	if (errcode) return errcode; //graph corrupted
	exists = true;

	return 0;
}
	

void Schema::getInterfaceForInterfaceOrObjectName(string name, bool &exists, Schema *&out)
{
	exists = false;
	bool hasInterface = InterfaceMaps::Instance().hasInterface(name);
	if (hasInterface)
	{
		out = new Schema(InterfaceMaps::Instance().getSchemaForInterface(name));
		exists = true;
		return;
	}
	bool hasObjectName = InterfaceMaps::Instance().hasObjectName(name);
	if (hasObjectName)
	{
		string iName = InterfaceMaps::Instance().getInterfaceNameForObject(name, exists);
		if (exists)
		{
			out = new Schema(InterfaceMaps::Instance().getSchemaForInterface(iName));
		}
	}
}

int Schema::getClassForClassOrObjectName(string name, TManager::Transaction *tr, bool &exists, Schema *&out)
{
	exists = false;
	int errcode = getClassByName(name, exists, out);
	if (errcode) return errcode;
	if (exists) return 0;
	
	vector<LogicalID*>* classLids;
	errcode = tr->getClassesLIDByInvariant(name, classLids);
	if (errcode) return errcode;
	if (classLids->size() == 0) return 0;
	return getClassByStoreLid(classLids->at(0), exists, out);
}

int Schema::getImplementationForName(string name, TManager::Transaction *tr, bool &exists, int &type, Schema *&out)
{
	exists = false;
	getInterfaceForInterfaceOrObjectName(name, exists, out);
	if (exists)	{type = BIND_INTERFACE; return 0;}
	
	int errcode = viewFromName(name, tr, exists, out);
	if (errcode) return errcode;
	if (exists) {type = BIND_VIEW; return 0;}

	errcode = getClassForClassOrObjectName(name, tr, exists, out);
	if (errcode) return errcode;
	if (exists) {type = BIND_CLASS; return 0;}

	return 0;
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

int Matcher::FindImpBoundToInterface(const string& interfaceObjectName, TManager::Transaction *tr, string& interfaceName, string &impName, string &impObjName, bool &found, int &t)
{
    int errcode = tr->getInterfaceBindForObjectName(interfaceObjectName, interfaceName, impName);
    found = false;
	if (interfaceName.empty())
    {   //nie ma takiego interfejsu (interfaceObjectName nie jest nazwa obiektu zadnego interfejsu)
		return (ErrQExecutor | ENoInterfaceFound);
	}
	else if (errcode)
	{
		return errcode;
    }
    else
    {
		if (impName.empty())
		{	//jest to nazwa obiektow spelniajacych interfejs, ale interfejs jest nie zwiazany z implementacja
			return 0;
		}
		else 
		{
			Schema s;
			Schema *sp = &s;
			errcode = Schema::getCIVObject(impName, tr, found, t, sp);
			if (errcode) return errcode;
			if (found) {impObjName = sp->getAssociatedObjectName();}
		}
	}
	return 0;
}






